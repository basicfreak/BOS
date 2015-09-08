[bits 16]
[org 0]

xchg bx, bx
xor ax, ax
mov ax, 0x13E0
mov es, ax
mov ds, ax
mov fs, ax
mov gs, ax
mov si, TESTMSG
call puts
cli
hlt
jmp $

%include "./COMMON/stdio_16.inc"
;----------------------------------------------------
;                     Variables
;----------------------------------------------------
	TESTMSG					DB "TEST On Fat Driver COMPLETED",0