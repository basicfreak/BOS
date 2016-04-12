; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                          System Initialization x86_64
;                          04/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 32

global init_x64
extern ERROR

%include 'linker.inc'
%include 'term.inc'
%include 'data/rodata.inc'
%include 'CPUID.inc'

init_x64:
	mov si, MSG.PDIR
	call puts32

	mov edi, 0x200000					; 0 Out Temporary Page Directory Memory
	mov ecx, 0xE00
	xor eax, eax
	rep stosd

	mov edi, 0x200000					; Setup Temporary Page Directory
	mov eax, 0x200003
	mov [edi + 0xFF0], eax				; Recursive
	add eax, 0x1004
	mov [edi], eax						; Lower Half (0)
	mov [edi + 0xFF8], eax				; Higher Half (-512GB)
	add edi, 0x1000
	add eax, 0x1000
	mov [edi], eax						; Lower Half (0 and -512GB)
	mov [edi + 0xFF8], eax				; Higher Half (-1GB)
	add edi, 0x1000						; Map first GB to 0, -512GB, and -1GB
	mov eax, 0x87
	mov ecx, 511
	.PSELoop:
		stosd
		add edi, 4
		add eax, 0x200000
		loop .PSELoop
	mov eax, 0x206003					; This is a Page Table at the very end..
	stosd

	mov si, MSG.Done
	call puts32
	mov si, MSG.LM64
	call puts32
	mov edx, LM_Entry					; PM to LM
	mov ebx, 0x200000
	mov al, 3
	jmp AP_Strap

bits 64

LM_Entry:
	mov si, MSG.Done
	call puts64
	mov si, MSG.PMM
	call puts64

	mov esi, Files.LM_PMM				; Load Physical Memory Manager
	mov edi, 0x100000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts64
	mov si, MSG.VMM
	call puts64

	mov esi, Files.LM_VMM				; Load Virtual Memory Manager
	mov edi, 0x108000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts64
	mov si, MSG.Linker
	call puts64

	mov esi, Files.LM_Link				; Load BOS Linker / Builder
	mov edi, 0x110000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts64

	xchg bx, bx
	mov rax, 0xFFFFFFFFC0110000			; Call BOS Linker / Builder
	mov rbx, BSP.Vendor
	call rax
	ret									; Return if we get back here...

	.Error:
		mov si, MSG.Fail
		call puts64
		ret

FILE_IO_WRAP:
	mov al, 4							; LM to CM32
	mov edx, .PMEnt
	jmp AP_Strap
bits 32
	.PMEnt:
		xor eax, eax					; FILE_IO (Open)
		call FILE_IO
		jc .NotFound					; If Error, Not Found
		mov eax, 1						; FILE_IO (Read)
		call FILE_IO
		jc .NotFound					; If Error, Not Found
		mov eax, 3						; FILE_IO (Close)
		call FILE_IO
		mov edx, .Found					; We found, and loaded, the file
		jmp .cont
		.NotFound:
			mov edx, .NFound			; We did not find or load the file
		.cont:
			mov al, 3					; PM to LM
			mov ebx, 0x200000
			jmp AP_Strap
bits 64
	.NFound:
		stc								; CF = 1 on Error
	.Found:
		ret								; Return
