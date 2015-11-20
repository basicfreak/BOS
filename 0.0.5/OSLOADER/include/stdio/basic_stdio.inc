%ifndef __BASIC_STDIO_INC__INCLUDED__
%define __BASIC_STDIO_INC__INCLUDED__

[bits 16]

puts:
	pusha
	.PutsLoop:
		lodsb
		or al, al
		jz PutsDone ; Check for Terminator (0)
		mov ah, 0eh
		int 10h ; Print Charactor To Screen (INT 10h 0Eh)
		jmp .PutsLoop
	PutsDone:
		popa
		ret

%endif