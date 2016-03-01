; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                                   CPU Strap
;                          17/02/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 16

section .CPUSection
global AP_Strap

; AL = Function
; (E/R)DX = Destination
; EBX = Page Dir (0 for none)

; AL
; 0 = RM to PM
; 1 = PM to RM
; 2 = RM to LM
; 3 = PM to LM
; 4 = LM to PM
; 5 = LM to RM
AP_Strap:
	cmp al, 0
	je short .RMtoPM
	cmp al, 1
	je short .PMtoRM
	cmp al, 2
	je short .RMtoLM
	cmp al, 3
	je short .PMtoLM
	cmp al, 4
	je short .LMtoPM
	cmp al, 5
	je short .LMtoRM
	hlt
	jmp -2

	.RMtoPM:
		cli
		mov eax, cr0
		or al, 1
		mov cr0, eax
		jmp 0x18:.inPM
	.RMtoLM:
		cli
		mov cr3, ebx
		xchg ebx, edx
		mov ecx, 0xC0000080
		rdmsr
		or ax, 0x100
		wrmsr
		xchg ebx, edx
		mov eax, cr4
		or eax, 0x30
		mov cr4, eax
		mov eax, cr0
		or eax, 0x80000001
		mov cr0, eax
		jmp 0x38:.inLM
		ret


bits 64
	.LMtoPM:
		push 0x18
		mov eax, .inCapMode
		push rax
		retf
	.LMtoRM:
		push 0x18
		mov eax, .inCapModeToRM
		push rax
		retf
	.inLM:
		mov ax, 0x40
		mov ds, ax
		mov es, ax
		mov ss, ax
		jmp rdx
bits 32
	.PMtoRM:
		jmp 0x08:.inPRM
	.PMtoLM:
		mov cr3, ebx
		xchg ebx, edx
		mov ecx, 0xC0000080
		rdmsr
		or ax, 0x100
		wrmsr
		xchg ebx, edx
		mov eax, cr4
		or eax, 0x30
		mov cr4, eax
		mov eax, cr0
		or eax, 0x80000001
		mov cr0, eax
		jmp 0x38:.inLM
		
bits 32
	.inPM:
		mov ax, 0x20
		mov ds, ax
		mov es, ax
		mov ss, ax
		jmp edx
	.inCapMode:
		mov ax, 0x20
		mov ds, ax
		mov es, ax
		mov ss, ax
		xchg ebx, edx
		mov ecx, 0xC0000080
		rdmsr
		and ax, 0xFEFF
		wrmsr
		xchg ebx, edx
		mov eax, cr0
		and eax, 0x7FFFFFFF
		mov cr0, eax
		xor eax, eax
		mov cr3, eax
		jmp edx
	.inCapModeToRM:
		mov ax, 0x20
		mov ds, ax
		mov es, ax
		mov ss, ax
		xchg ebx, edx
		mov ecx, 0xC0000080
		rdmsr
		and ax, 0xFEFF
		wrmsr
		xchg ebx, edx
		mov eax, cr0
		and eax, 0x7FFFFFFF
		mov cr0, eax
		xor eax, eax
		mov cr3, eax
		jmp 0x08:.inPRM

bits 16
	.inPRM:
		mov ax, 0x10
		mov ds, ax
		mov es, ax
		mov ss, ax
		mov eax, cr0
		and eax, 0x7FFFFFFE
		mov cr0, eax
		jmp 0:.inRM
	.inRM:
		xor ax, ax
		mov ds, ax
		mov es, ax
		mov ss, ax
		sti
		jmp DWORD edx
