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
	xchg bx, bx
	call _VMM_init
	cli
	xchg bx, bx
	hlt
	jmp $

