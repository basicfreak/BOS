bits 64
default rel

MINIMUM_RAM		equ 0x2000000			; 32 MB Minimum

extern FILE_IO
extern AP_Strap
extern end
extern _PMM_init
extern _VMM_init

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
	xchg bx, bx



	.Error:
		xchg bx, bx
		; ret
		hlt
		jmp $

