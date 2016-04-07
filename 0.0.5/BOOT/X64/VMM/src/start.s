bits 64
default rel

extern FILE_IO
extern AP_Strap
extern end

section .init
global _init

_init:
	xchg bx, bx
	cli
	hlt
	jmp $
