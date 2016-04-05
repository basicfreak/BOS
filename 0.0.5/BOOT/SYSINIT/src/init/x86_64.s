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

	mov eax, 0x20100B
	mov edi, 0x200000
	stosd
	add eax, 0x1000
	mov edi, 0x201000
	stosd
	mov ecx, 512
	mov edi, 0x202000
	mov eax, 0x8B
	.PSELoop:
		stosd
		add eax, 0x200000
		add edi, 4
		loop .PSELoop
	mov eax, 0x200000
	mov ecx, eax
	add ecx, 3
	mov DWORD [eax + 0xFF0], ecx
	add ecx, 0x1000
	mov DWORD [eax + 0xFF8], ecx

	mov si, MSG.Done
	call puts32
	mov si, MSG.LM64
	call puts32
	mov edx, LM_Entry
	mov ebx, 0x200000
	mov al, 3
	jmp AP_Strap

bits 64

LM_Entry:
	mov si, MSG.Done
	call puts64
	mov si, MSG.PMM
	call puts64

	mov esi, Files.LM_PMM
	mov edi, 0x100000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts64
	mov si, MSG.VMM
	call puts64

	mov esi, Files.LM_VMM
	mov edi, 0x108000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts64
	mov si, MSG.Linker
	call puts64

	mov esi, Files.LM_Link
	mov edx, 0x110000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts64

	mov rax, 0xFFFFFF8000110000
	mov rbx, BSP.Vendor
	call rax
	ret

	.Error:
		mov si, MSG.Fail
		call puts64
		ret

FILE_IO_WRAP:
	mov al, 4
	mov edx, .PMEnt
	jmp AP_Strap
bits 32
	.PMEnt:
		xor eax, eax
		call FILE_IO
		jc .NotFound
		mov eax, 1
		call FILE_IO
		jc .NotFound
		mov edx, .Found
		jmp .cont
		.NotFound:
			mov edx, .NFound
		.cont:
			mov al, 3
			mov ebx, 0x200000
			jmp AP_Strap
bits 64
	.NFound:
		stc
	.Found:
		ret
