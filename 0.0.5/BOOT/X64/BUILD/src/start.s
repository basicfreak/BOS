bits 64
default rel

extern FILE_IO
extern AP_Strap
extern end
extern _PMM_init
extern _VMM_init

global start

start:
	xchg bx, bx
	call _PMM_init
	jc .Error
	xchg bx, bx
	call _VMM_init
	jc .Error
	xchg bx, bx



	.Error:
		xchg bx, bx
		ret
		hlt
		jmp $

