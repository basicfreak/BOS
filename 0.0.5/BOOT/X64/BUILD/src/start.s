bits 64
default rel

MINIMUM_RAM		equ 0x2000000			; 32 MB Minimum

extern FILE_IO
extern AP_Strap
extern end
extern _PMM_init
extern _VMM_init
extern _VMM_map
extern _VMM_unmap

global start

start:
	; xchg bx, bx
	call _PMM_init
	jc .Error
	cmp rax, MINIMUM_RAM
	jb .Error
	; xchg bx, bx
	call _VMM_init
	jc .Error
	mov rdx, 0xFFFF80C0A001F000
	mov eax, 0xFF0
	mov rbx, 3
	call _VMM_map
	xchg bx, bx
	mov rax, 0xFFFF80C0A001F000
	call _VMM_unmap



	.Error:
		xchg bx, bx
		ret
		hlt
		jmp $

