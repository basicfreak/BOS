; i686: Task Management and IDT Handler
; NOTE: This is full of Black Magic and Stack Tricks

; I assume NO responsablity for any damage (including but not limited to hardware,
; software or data) resulted from use of this code (compiled or otherwise). The
; following code is provided as-is with no warranty of any sort.

; ALSO NOTE: I have no clue how GCC will react if linked to the following in any
;			 way, as no function provided is C complient.

[bits 32]

[global IDT_COMMON]
[global ThreadManager]
[global LoadThread]
[extern CurrentThread]
[extern LastThread]
[extern IPC_Handler]
[extern MEM_Handler]
[extern API_Handler]
[extern ForkThread]
[extern ExecThread]
[extern _VMM_PageFaultManager] ; _VMM_PageFaultManager(regs *r)
[extern killCurrentThread] ; killCurrentThread(regs *r)

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
    mov eax, esp
    push eax
    push .HandlerReturn  ; push return address (hack I Know)
;xchg bx, bx
	; OLD CALL TO C HANDLER
    ;mov eax, IDT_HANDLER
    ;call eax

    mov eax, [esp + 56] ; Get INT_NUMBER
    cmp eax, 0x20
    jl EXCEPTION_HANDLER
    cmp eax, 0xF0
    jl INT_HANDLER
    je IPC_Handler
    cmp eax, 0xF1
    je TM_Handler
    cmp eax, 0xF2
    je MEM_Handler
    cmp eax, 0xF3
    je API_Handler

	.HandlerReturn:
	    pop eax
	    pop gs
	    pop fs
	    pop es
	    pop ds
	    popa
	    add esp, 8
	;xchg bx, bx
	    iretd

EXCEPTION_HANDLER:
	cmp eax, 0x0E
	je _VMM_PageFaultManager
	cmp eax, 0x0D
	je KILL_SYSTEM
	jmp killCurrentThread

INT_HANDLER:
	cmp eax, 0x2F
	jg .HandlerStart
	push eax
	cmp eax, 0x28
	mov al, 0x20
	jl .CleareMasterPIC
	out 0xA0, al
	.CleareMasterPIC:
		out 0x20, al
		pop eax
	.HandlerStart:
		shl eax, 2
		add eax, DWORD [INTList]
		mov edx, [eax]
		cmp edx, 0 ; Do we know a task?
		je .setFired
		cmp edx, 0xFFFFFFFF
		je .done ; It already fired once and set.

		pop ecx ; Pop old return address
		push .INT_Return
		jmp LoadThread

		.INT_Return:
			mov DWORD [INTList + (eax * 4)], 0
			jmp IDT_COMMON.HandlerReturn
	.setFired:
		mov DWORD [eax], 0xFFFFFFFF
	.done:
		jmp ThreadManager
		ret

KILL_SYSTEM:
	cli
	hlt
	jmp KILL_SYSTEM

TM_Handler:
	mov eax, DWORD [esp + 52]
	cmp al, 0x00
	je killCurrentThread
	cmp al, 0x01
	je ForkThread
	cmp al, 0x02
	je ExecThread
	cmp al, 0x03
	je ThreadManager
	cmp al, 0x80
	je .ssefpu
	cmp al, 0x81
	je .ioperm

	mov DWORD [esp + 52], 0
	ret
	.ssefpu:
		mov edx, DWORD [CurrentThread]
		shl edx, 10 ; Times 1024
		add edx, DWORD [TaskList]
		or DWORD [edx + 76], 0x00000010
		ret
	.ioperm:
		or DWORD [esp + 72], 0x00003000 ; set eflags to allow io
		ret

ThreadManager:
;xchg bx, bx
	mov edx, DWORD [LastThread]
	; Uncomment next 2 lines for Cooperative Multitasking
	cmp edx, DWORD [CurrentThread]
	jne .Done

	mov ecx, edx
	shl ecx, 10
	add ecx, DWORD [TaskList]

	.loop:
		inc edx
		cmp edx, 0x1000 ; Max Threads = 4096
		jg .resetloop
		add ecx, 0x400
		mov ebx, DWORD [ecx + 76]
		cmp edx, DWORD [LastThread]
		je .StartIdle
		
		;test ebx, ebx
		;jz .resetloop
		and ebx, 1
		test ebx, ebx
		jnz .Done
		jmp .loop

		.resetloop:
			xor edx, edx
			mov ecx, DWORD [TaskList]
			jmp .loop

	.StartIdle:
		and ebx, 1
		test ebx, ebx
		jnz .Done
		xor edx, edx
	.Done:
		mov DWORD [LastThread], edx
		
LoadThread:
;xchg bx, bx
	; INPUT EDX = next Task & Last stack item is regs pointer

	; Save Current Thread
	mov esi, DWORD [esp + 4]
	;add esi, 28
	mov edi, DWORD [CurrentThread]
	shl edi, 10 ; Times 1024
	add edi, DWORD [TaskList]
	mov ecx, 19 ; Size of Regs (in DWORDs)
	rep movsd
	mov ecx, DWORD [esi]
	and ecx, 0x00000010
	test ecx, ecx
	jz .SetNextThread
	fxsave [edi + 432]

	.SetNextThread:
		mov DWORD [CurrentThread], edx ; Save New Thread Number As Current

	; Load New Current Thread
	mov esi, DWORD [CurrentThread]
	shl esi, 10 ; Multiply by 1024
	add esi, DWORD [TaskList] ; Add Base Pointer
	mov edi, DWORD [esp + 4] ; Regs Location
	;add edi, 28
	mov ecx, 19
	rep movsd
	cmp DWORD [esi + 4], 0
	jz .SkipDir
	mov ecx, DWORD [esi + 4]
	mov cr3, ecx
	.SkipDir:
	mov ecx, DWORD [esi]
	and ecx, 0x00000010
	test ecx, ecx
	jz .SkipSSE
	fxrstor [esi + 432]
	.SkipSSE:
	ret

;typedef struct ThreadEntry {
;	regs TRegs; // 76 Bytes
;	uint32_t Flags;
;	uint32_t CR3;
;	char Name[428];
;	uint8_t SSEData[512]; // 512 Bytes
;}__attribute__((packed)) Thread_t, *Thread_p; // 1KB

TaskList dd 0xE0001000
;MessageList dd 0
INTList dd 0xE0000000
;APIList dd 0
;CurrentThread dd 0
;LastThread dd 0