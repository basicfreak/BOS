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
[global killCurrentThread]
[global KILL_SYSTEM]

[extern CurrentThread]
[extern LastThread]

[extern ForkThread]
[extern ExecThread]
[extern _VMM_PageFaultManager] ; _VMM_PageFaultManager(regs *r)
[extern _VMM_PageFaultHandler]

[extern _PMM_alloc]
[extern _VMM_map]
[extern _VMM_getPhys]
[extern _VMM_newPDir]
[extern _VMM_mapOther]
[extern _VMM_getPhysOther]
[extern _PMM_free]
[extern _VMM_umap]
[extern _VMM_umapOther]
[extern _VMM_umap_UNSAFE]
[extern _VMM_map_UNSAFE]

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
;xchg bx, bx

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
;xchg bx, bx
	    iretd									; Return to thread.

;-------------------------------------------------------------------------------
;                            CPU Exception Handling
;-------------------------------------------------------------------------------
EXCEPTION_HANDLER:
	cmp eax, 0x0E								; Is this a Page Fault?
	;je _VMM_PageFaultManager					; Yes? Run Page Fault Manager.
	je _VMM_PageFaultHandler
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
;xchg bx, bx
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

MEM_Handler:
	mov eax, DWORD [esp + 52]					; Grab Function Number (stored in eax of the calling thread)
	cmp al, 0x00								; Are we requesting new page?
	je .AllocMap								; Yes? Allocate and Map Page.
	cmp al, 0x01								; Are we unmapping and freeing page?
	je .FreeUMap								; Yes? Free And Unmap Page.
	cmp al, 0x80								; Are We mapping a known address?
	je .Map										; Yes? Map Page.
	cmp al, 0x81								; Are we unmapping a page?
	je .UMap									; Yes? Unmap page.
	cmp al, 0x8F								; Find Physical Address?
	je .FindPhys								; Yes? Find Address.
	cmp al, 0xF0								; Create new Dir?
	je .NewDir									; Yes? Make Dir.
	cmp al, 0xF1								; Are We mapping a known address on another pdir?
	je .MapOther								; Yes? Map page in pdir.
	cmp al, 0xF2								; Are we requesting new page on another pdir?
	je .AllocMapOther							; Yes? Allocate and Map page for pdir.
	cmp al, 0xF3								; Are we unmapping a page in another pdir?
	je .UMapOther								; Yes? Unamp page in pdir.
	cmp al, 0xF4								; Are we unmapping and freeing a page in another pdir?
	je .FreeUMapOther							; Yes? Free and Unmap Page in pdir.
	cmp al, 0xFF								; Are we requesting physical address in another pdir?
	je .FindPhysOther							; Yes? Get physical address from pdir.
	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread

	.AllocMap:
;	_VMM_map((void*)r->edx, _PMM_alloc(PAGESIZE), TRUE, (bool)r->ebx);
		push DWORD 0x1000						; Push PAGESIZE
		call _PMM_alloc							; Call Physical Memory Allocator
		mov ecx, eax							; Store address in ecx
		pop eax									; Clean Stack
	.Map:
;	_VMM_map((void*)r->edx, (void*)r->ecx, TRUE, (bool)r->ebx);
		;push ebx								; Push Write Flag
		;push DWORD 1							; Push TRUE, this is a user page
		;push ecx								; Push Physical Address
		;push DWORD [esp + 56]					; push r->edx (this is changed if page is allocated)
		;call _VMM_map							; Map Page
		;add esp, 16								; Clean Stack
		;eax KERNEL Flag
		;ecx Physical
		;edx Virtual
		;ebx WriteFlag
		xor eax, eax
		mov edx, DWORD [esp + 44]
		call _VMM_map_UNSAFE
		ret										; Return to thread

	.FreeUMap:
;	_PMM_free(_VMM_getPhys((void*)r->edx), PAGESIZE);
		push edx								; Push Virutal Address r->edx
		call _VMM_getPhys						; Call Get Physical Address Function (returns eax)
		push DWORD 0x1000						; Push PAGESIZE
		push eax								; Push Physical Address
		call _PMM_free							; Free The Page
		add esp, 8								; Clean Stack
		pop edx
	.UMap:
;	_VMM_umap((void*)r->edx);
		;push DWORD [esp + 44]					; Push Virutal Address r->edx (changed if we free the page first)
		;call _VMM_umap							; unmap the page.
		;pop eax									; Clean Stack
		call _VMM_umap_UNSAFE
		ret										; Return to thread

	.FindPhys:
;	r->eax = (uint32_t) _VMM_getPhys((void*)r->edx);
		push edx								; Push Virtual Address r->edx
		call _VMM_getPhys						; Get Physical Address
		mov DWORD [esp + 56], eax				; Save Physical Address in r->eax
		pop eax									; Clean Stack
		ret										; Return to thread

	.NewDir:
;	r->eax = (uint32_t) _VMM_newPDir();
		call _VMM_newPDir						; Get New Page Directory
		mov DWORD [esp + 52], eax				; Save Page Directory Address in r->eax
		ret										; Return to thread

	.AllocMapOther:
;	_VMM_mapOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx, _PMM_alloc(PAGESIZE), TRUE, (bool) r->ebx);
		push eax								; Save eax
		push DWORD 0x1000						; Push Page Size
		call _PMM_alloc							; Allocate Page
		mov ecx, eax							; Save Page in ecx
		pop eax									; Clean Stack
		pop eax									; Restore eax
	.MapOther:
;	_VMM_mapOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx, (void*) r->ecx, TRUE, (bool) r->ebx);
		and eax, 0xFFFFF000						; Remove Function Number From eax
		push ebx								; Push Write Flag
		push DWORD 1							; Push TRUE (user flag)
		push ecx								; Push Physical Address
		push DWORD [esp + 56]					; Push Virtual Address r->edx ( as this is changed if we allocated )
		push eax								; Push Page Directory Address
		call _VMM_mapOther						; Map Page
		add esp, 20								; Clean Stack
		ret										; Return to thread

	.FreeUMapOther:
;	_PMM_free(_VMM_getPhysOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx), PAGESIZE);
	.UMapOther:
;	_VMM_umapOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx);
		ret

	.FindPhysOther:
;	r->eax = (uint32_t) _VMM_getPhysOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx);
		and eax, 0xFFFFF000						; Remove Function Number From eax
		push edx								; Push Virtual Address
		push eax								; Push Page Directory Address
		call _VMM_getPhysOther					; Get Physical Address
		mov DWORD [esp + 60], eax				; Save Physical Address in r->eax
		add esp, 8								; Clean Stack
		ret										; Return to thread

IPC_Handler:
	mov eax, DWORD [esp + 52]					; Grab Function Number (stored in eax of the calling thread)
	cmp al, 0x00								; Wait for message?
	je .WaitMSG									; Yes? Handle Flags.
	cmp al, 0x01								; Send a message?
	je .SendMSG									; Yes? Send Message.
	cmp al, 0x80								; Wait for IRQ/INT?
	je .WaitINT									; Yes? Handle Flags.
	cmp al, 0x81								; Wait for IRQ/INT with regs?
	je .WaitINTwr								; Yes? Handle Flags.
	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread
	.WaitMSG:

; TO DO ADD MESSAGING SYSTEM

		ret
	.SendMSG:

; TO DO ADD MESSAGING SYSTEM

		ret
	.WaitINTwr:
		mov edx, DWORD [CurrentThread]			; CurrentThread ID
		shl edx, 10								; Multiply by 1KB
		add edx, DWORD [TaskList]				; Add ThreadList Base
		or DWORD [edx + 76], TF_REGS			; Set Regs Flag
	.WaitINT:
		mov eax, DWORD [esp + 40]				; Threads EBX (int_no we are waiting for)
		shl eax, 2								; Multiply int_no by 4 (4 bytes per DWORD)
		add eax, DWORD [INTList]				; Add base address for int list to int_no
		mov edx, [eax]							; Grab thread ID or Flag
		cmp edx, 0xFFFFFFFF						; Has this INT Fired Yet?
		je .FiredAlready						; Yes?
		mov edx, DWORD [CurrentThread]			; CurrentThread ID
		mov [eax], edx							; Save CurrentThread ID in INT Table.
		shl edx, 10								; Multiply by 1KB
		add edx, DWORD [TaskList]				; Add ThreadList Base
		and DWORD [edx + 76], TF_DEAD			; Clear Active Flag
		or DWORD [edx + 76], 0x00000004			; Set Wait for INT Flag
		jmp ThreadManager						; Yeild The Thread if INT not ready.
	.FiredAlready:
		mov DWORD [eax], 0						; Clear Fired Flag
		ret										; Return to Thread


API_Handler:
	mov eax, DWORD [esp + 52]					; Grab Function Number (stored in eax of the calling thread)
	; TO DO ADD API
	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread.

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
;xchg bx, bx
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
;xchg bx, bx
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

;typedef struct ThreadEntry {
;	regs TRegs; // 76 Bytes
;	uint32_t Flags;
;	uint32_t CR3;
;	char Name[428];
;	uint8_t SSEData[512]; // 512 Bytes
;}__attribute__((packed)) Thread_t, *Thread_p; // 1KB



;-------------------------------------------------------------------------------
;                                   Variables
;-------------------------------------------------------------------------------
TaskList dd 0xE0001000							; Virtual Address of Thread List.
;MessageList dd 0
INTList dd 0xE0000000							; Virtual Address of INT Waiting Threads.
;APIList dd 0
;CurrentThread dd 0
;LastThread dd 0
