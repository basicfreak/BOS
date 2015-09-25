;-------------------------------------------------------------------------------
;                                 taskman_asm.s
;                                   BOS 0.0.4
;                                  2015-09-15
;                                Brian T. Hoover
;
; i686: Task Management and IDT Handler
; NOTE: This is full of Black Magic and Stack Tricks
;
; I assume NO responsablity for any damage (including but not limited to hardware,
; software or data) resulted from use of this code (compiled or otherwise). The
; following code is provided as-is with no warranty of any sort.
;
; ALSO NOTE: I have no clue how GCC will react if linked to the following in any
;			 way, as no function provided is C complient.
;-------------------------------------------------------------------------------


[bits 32]

[global IDT_COMMON]
[global ThreadManager]
[global LoadThread]
[global killCurrentThread]
[global KILL_SYSTEM]
[global TaskList]
[global INTList]
[global TF_ACTIVE]
[global TF_USESSE]
[global TF_WAITINT]
[global TF_DEAD]
[global TF_WAITMSG]
[global TF_BLANK]
[global TF_REGS]
[global TF_LINKF]

[extern SendMessage]
[extern CurrentThread]
[extern LastThread]
[extern API_Handler]
[extern IPC_Handler]
[extern MEM_Handler]
[extern ForkThread]
[extern ExecThread]
[extern _VMM_PageFaultHandler]

;-------------------------------------------------------------------------------
;                                   Constants
;-------------------------------------------------------------------------------
MAX_THREADS equ 0x1000
MAX_MSGS equ 0x800
TF_ACTIVE equ 0x00000001
TF_WAITINT equ 0x00000004
TF_WAITMSG equ 0x00000008
TF_USESSE equ 0x00000010
TF_DEAD equ 0xFFFFFFFE
TF_REGS equ 0x00000080
TF_LINKF equ 0xFFFFFC00
TF_BLANK equ 0xFF00FF00
INT_FLAGED equ 0xFFFFFFFF

;-------------------------------------------------------------------------------
;                          Standard INT Handler Rutine
;-------------------------------------------------------------------------------
IDT_COMMON:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp								; Get regs base.
    push eax									; push regs base.
    push .HandlerReturn							; push return address (hack I Know)
    mov eax, [esp + 56]							; Get int_no
    cmp eax, 0x20								; Is this CPU Generated?
    jl EXCEPTION_HANDLER						; Yes? Load Exception Handler.
    cmp eax, 0xF0								; Is this an irq/int? AND Is this IPC Call?
    jl INT_HANDLER								; Yes, irq/int? Load INT Handler.
    je IPC_Handler								; Yes, IPC Call? Load IPC Handler.
    cmp eax, 0xF1								; Is this a TM Call?
    je TM_Handler								; Yes? Load TM Handler.
    cmp eax, 0xF2								; Is this a MEM Call?
    je MEM_Handler								; Yes? Load Memory Manager.
    cmp eax, 0xF3								; Is this an API Call?
    je API_Handler								; Yes? Load API Manager.
	.HandlerReturn:
	    pop eax
	    pop gs
	    pop fs
	    pop es
	    pop ds
	    popa
	    add esp, 8
	    iretd									; Return to thread.

;-------------------------------------------------------------------------------
;                            CPU Exception Handling
;-------------------------------------------------------------------------------
EXCEPTION_HANDLER:
	cmp eax, 0x0E								; Is this a Page Fault?
	je _VMM_PageFaultHandler					; Yes? Run Page Fault Manager.
	cmp eax, 0x0D								; Is this a General Protection Fault?
	je KILL_SYSTEM								; Yes? Kill The System. (temperary response to GPF)
	jmp killCurrentThread						; Else? Kill Current Thread.

;-------------------------------------------------------------------------------
;                            Normal IRQ/INT Handling
;-------------------------------------------------------------------------------
INT_HANDLER:
	cmp eax, 0x2F								; See if this came from PIC
	jg .HandlerStart
	push eax									; save int_no
	cmp eax, 0x28								; See if this came from Slave PIC
	mov al, 0x20								; PIC ACK Command
	jl .CleareMasterPIC
	out 0xA0, al								; ACK Slave PIC
	.CleareMasterPIC:
		out 0x20, al							; ACK Master PIC
		pop eax									; restore int_no

	.HandlerStart:
		shl eax, 2								; Multiply int_no by 4 (4 bytes per DWORD)
		add eax, DWORD [INTList]				; Add base address for int list to int_no
		mov edx, [eax]							; Grab thread ID or Flag
		cmp edx, 0								; Do we have a thread ID?
		je .setFlag								; No? Flag it.
		cmp edx, INT_FLAGED						; Is it flagged?
		je .done								; It already fired once and set.
		pop ecx									; Pop old return address (another Hack)
		cmp edx, DWORD [CurrentThread]			; Is this INT going to Current Thread?
		je .INT_Return							; Yes? We are done here
		push .INT_Return						; Push new return address (tet another Hack)
		jmp LoadThread							; Lets Load the Thread
		.INT_Return:
			mov DWORD [eax], 0					; Clear any flag or ID
			mov edx, DWORD [CurrentThread]		; CurrentThread ID
			shl edx, 10							; Multiply by 1KB
			add edx, DWORD [TaskList]			; Add ThreadList Base
			and DWORD [edx + 76], 0xFFFFFF7B	; Clear Wait For INT and Regs Flags
			or DWORD [edx + 76], TF_ACTIVE		; Set Active Flag
			jmp IDT_COMMON.HandlerReturn		; Return to loaded thread
	.setFlag:
		mov DWORD [eax], INT_FLAGED				; Set Flag
	.done:
		jmp ThreadManager						; Locate New Thread

;-------------------------------------------------------------------------------
;                                 System Calls
;-------------------------------------------------------------------------------
TM_Handler:
	mov eax, DWORD [esp + 52]					; Grab Function Number (stored in eax of the calling thread)
	cmp al, 0x00								; Kill?
	je killCurrentThread						; Yes? Kill the current thread.
	cmp al, 0x01								; Fork?
	je ForkThread								; Yes? Fork current thread.
	cmp al, 0x02								; Exec?
	je ExecThread								; Yes? Exec new thread.
	cmp al, 0x03								; Yeild?
	je ThreadManager							; Yes? Look for new thread.
	cmp al, 0x80								; SSE/FPU?
	je .ssefpu									; Yes?
	cmp al, 0x81								; IOPERM?
	je .ioperm									; Yes?
	cmp al, 0xF0								; Request thread ID by name?
	je .RequestID								; Yes?
	cmp al, 0xFF								; Request Name Change?
	je .RenameThread							; Yes?

	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread.
	.ssefpu:
		mov edx, DWORD [CurrentThread]			; CurrentThread ID
		shl edx, 10								; Multiply by 1KB
		add edx, DWORD [TaskList]				; Add ThreadList Base
		or DWORD [edx + 76], TF_USESSE			; Set flag so ThreadManager saves and loads SSE/FPU regs
		ret										; Return to thread.
	.ioperm:
		or DWORD [esp + 72], 0x00003000			; set eflags to allow io
		ret										; Return to thread.
	.RequestID:
		mov ebp, esi							; Save Pointer to Name
		xor ecx, ecx							; Clear Counter Register
		xor edx, edx							; Clear Data (Thread ID) Register
		.StrLen:
			lodsb								; Grab a character from Name Pointer
			or al, al							; Check for 0
			jz .FindName						; If 0 we have found length.
			inc ecx								; Else increment Counter Register
			jmp .StrLen							; Loop while *Name != 0
		.FindName:
			mov ebx, ecx						; Save Count in Base Register
			.IDLoop:
				mov esi, ebp					; Restore Name Pointer
				mov ecx, ebx					; Restore Count Register
				mov edi, edx					; Calculate Destination (Thread to test in EDX)
				shl edi, 10						; Multiply Destination by 1024
				add edi, DWORD [TaskList]		; Add Base of Task List
				cmp DWORD [edi + 76], TF_BLANK	; Check if we made it threw the list of threads.
				je .NotFound					; If we do, Fail.
				add edi, 84						; Add offset to Name in Thread
				rep cmpsb						; compare Name
				je .IDDone						; If equal we found a match
				inc edx							; Else Increment Thread Number
				jmp .IDLoop						; Loop until complete or Fail
			.IDDone:
				mov DWORD [esp + 52], edx		; Store Thread ID in current threads eax
				ret								; return to current thread
		.NotFound:
			mov DWORD [esp + 52], 0				; Store 0 in current threads eax
			ret									; return to current thread
	.RenameThread:
		;esi = new name pointer
		mov edi, DWORD [CurrentThread]			; Calculate Destination: Current Thread ID
		shl edi, 10								; Multiply by 1024
		add edi, DWORD [TaskList]				; Add Thread List Base
		add edi, 84								; Add Name offset
		.RenameLoop:
			lodsb								; Grab a character from Name Pointer
			or al, al							; Check for 0
			jz .RenameDone						; If 0 we finished the rename.
			mov BYTE [edi], al					; Else write character
			inc edi								; Increment Destination
			jmp .RenameLoop						; Loop until done
		.RenameDone:
			mov BYTE [edi + 1], 0				; Set NULL termenator
			ret									; Return to current thread.

;-------------------------------------------------------------------------------
;                               Special Functions
;-------------------------------------------------------------------------------
KILL_SYSTEM:
	cli											; Clear IF (should not be set anyways...)
	hlt											; Halt CPU
	jmp KILL_SYSTEM								; (should never get here) Loop the function

;-------------------------------------------------------------------------------
;                               Thread Management
;      THE FOLLOWING (EXCLUDING LoadThread) FUNCTIONS CAN BE CALLED FROM C
;    NOTE: NO REGISTERS ARE PRESERVED AND YOU BEST RETURN JUST AFTER CALLING
;-------------------------------------------------------------------------------
killCurrentThread:
	mov edx, DWORD [CurrentThread]				; CurrentThread ID
	shl edx, 10									; Multiply by 1KB
	add edx, DWORD [TaskList]					; Add ThreadList Base
	mov DWORD [edx + 76], TF_DEAD				; Set Flag to TF_DEAD (0xFFFFFFFE)

ThreadManager:
	mov edx, DWORD [LastThread]					; LastThread ID

	; Uncomment next 2 lines for Cooperative Multitasking
	;cmp edx, DWORD [CurrentThread]				; LastThread == CurrentThread?
	;jne .Done									; No? We are done here.

	mov ecx, edx								; Copy Thread ID.
	shl ecx, 10									; Multiply by 1KB.
	add ecx, DWORD [TaskList]					; Add ThreadList Base.

	.loop:
		inc edx									; Increment Thread ID
		cmp edx, 0x1000							; Have we passed max IDs?
		jge .resetloop							; Yes? Reset The Loop.
		add ecx, 0x400
		mov ebx, DWORD [ecx + 76]				; Store Thread Flag.

		cmp ebx, TF_BLANK						; Compare Flag to TF_BLANK
		je .resetloop							; If we are blank then reset loop

		cmp edx, DWORD [LastThread]				; Did we make the loop?
		je .StartIdle							; Yes? Start Idle or Resume LastThread.
		and ebx, TF_ACTIVE						; Clear all but Active Flag
		test ebx, ebx							; Is Thread ID Active?
		jnz .Done								; Yes? We are done here.
		jmp .loop								; No? Loop Again.

		.resetloop:
			xor edx, edx						; Set ThreadID to 0
			mov ecx, DWORD [TaskList]			; Reset to ThreadList Base
			jmp .loop							; Resume Loop.

	.StartIdle:
		and ebx, TF_ACTIVE						; Clear all but Active Flag
		test ebx, ebx							; Is LastThread Active?
		jnz .Done								; Yes? Resume LastThread
		xor edx, edx							; No? Start Idle Thread (ID 0)
	.Done:
		mov DWORD [LastThread], edx				; Save Thread ID as LastThread
		
LoadThread:
	; INPUT EDX = next Task & Last stack item (not including return address) is regs pointer

	; Save Current Thread
	mov esi, DWORD [esp + 4]					; Source = regs pointer
	mov edi, DWORD [CurrentThread]				; Destination = Current Thread
	shl edi, 10									;            Multiplied by 1KB
	add edi, DWORD [TaskList]					;       Plus the TaskList Base

	cmp DWORD [edi + 76], TF_DEAD				; Is Thread Dead?
	je .SaveComplete							; Yes? Do not save it.
	
	mov ecx, 19									; Size of Regs (in DWORDs)
	rep movsd									; Copy Regs
	mov ecx, DWORD [edi]						; Grab Thread Flags

	and ecx, TF_USESSE							; Clear all but SSE/FPU Flag
	test ecx, ecx								; Do I need to save SSE/FPU?
	jz .SaveComplete							; No? Save complete.
	fxsave [edi + 432]							; Save SSE/FPU regs.

	.SaveComplete:
		mov esi, edx							; Source = New Thread ID (edx)
		shl esi, 10								;           Multiplied by 1024
		add esi, DWORD [TaskList]				;       Plus The TaskList Base
		cmp DWORD [esi + 76], TF_DEAD			; Is New Thread Dead?
		jne .SetNextThread						; No? Good, Continue.
		xor edx, edx							; Yes? There is an issue load Idle Thread. (ID 0)
		mov esi, DWORD [TaskList]
	.SetNextThread:
		mov DWORD [CurrentThread], edx			; Save Thread ID in CurrentThread

	; Load New Current Thread
	
	mov edi, DWORD [esp + 4]					; Destination = regs pointer
	mov ecx, 19									; Size of Regs (in DWORDs)
	rep movsd									; Copy Regs
	cmp DWORD [esi + 4], 0						; Do we have a Page Directory?
	jz .SkipDir									; No? Skip Page Directory Load.
	mov ecx, DWORD [esi + 4]					; Get Page Directory from Thread.
	mov cr3, ecx								; Set Page Directory.
	.SkipDir:
		mov ecx, DWORD [esi]					; Get Thread Flags
		and ecx, TF_USESSE						; Clear all but SSE/FPU Flag
		test ecx, ecx							; Do I need to restore SSE/FPU?
		jz .SkipSSE								; No? Load Complete.
		fxrstor [esi + 432]						; Yes? Restore SSE/FPU regs.
	.SkipSSE:
		ret										; Return to Thread (usually)

;-------------------------------------------------------------------------------
;                                   Variables
;-------------------------------------------------------------------------------
TaskList dd 0xE0001000							; Virtual Address of Thread List.
;MessageList dd 0
INTList dd 0xE0000000							; Virtual Address of INT Waiting Threads.
;APIList dd 0
;CurrentThread dd 0
;LastThread dd 0
