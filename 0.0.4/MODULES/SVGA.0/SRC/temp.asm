; temp.asm

[bits 32]
[org 0x1000]

start:
xchg bx, bx
cli
sidt [tempidt]
jmp 0x28:urmode
[bits 16]
	urmode:
;		mov eax, cr3
;		mov DWORD [tempcr3], eax
		lidt [realidt]
;		mov eax, cr0
;		and eax, 0x7FFFFFFE
;		mov cr0, eax
;		jmp 0:rmode
;	rmode:
;		xor ax, ax
;		mov ds, ax
;		mov es, ax
		mov eax, 0x00004F02
		mov ebx, 0x00004114
		int 0x10
;		mov eax, DWORD [tempcr3]
;		mov cr3, eax
;		mov  eax, cr0          ; switch to pmode by
;		or eax,0x80000001                ; set pmode bit
;		mov  cr0, eax
		jmp 8:pmodetwo
[bits 32]
	pmodetwo:
;		mov ax, 0x10
;		mov ds, ax
;		mov es, ax
		lidt [tempidt]
		ret


realidt dw 0x3FF
		dd 0

tempidt dw 0
		dd 0

tempcr3 dd 0
