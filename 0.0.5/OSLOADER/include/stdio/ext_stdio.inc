%ifndef __EXT_STDIO_INC__INCLUDED__
%define __EXT_STDIO_INC__INCLUDED__

[bits 16]

; eax = ip from pxe
PrintIP:
	pushad
	mov si, IPStrBuff
	add si, 2
	mov ch, 4
	.loopIP:
		push eax
		mov bl, al
		xor eax, eax
		mov al, bl
		mov cl, 3
		.loopByte:
			xor edx, edx
			mov bx, 10
			div bx
			add dl, 0x30
			mov BYTE [ds:si], dl
			dec si
			dec cl
			jnz .loopByte
		pop eax
		shr eax, 8
		add si, 7
		dec ch
		jnz .loopIP
	mov si, IPStrBuff
	call puts
	popad
	ret

IPStrBuff db "000.000.000.000", 13, 10, 0

; input ax
PrintHex:
	pusha
	mov si, HexStrBuff
	add si, 5
	mov bx, 4
	.HexLoop:
		mov dl, al
		and dl, 0x0F
		cmp dl, 0xA
		jge .HexLetter
		add dl, '0'
		jmp .HexLooping
		.HexLetter:
			add dl, ('A' - 0xA)
		.HexLooping:
			mov [ds:si], dl
			dec bx
			test bx, bx
			jz .HexDone
			dec si
			shr ax, 4
			jmp .HexLoop
	.HexDone:
		dec si
		dec si
		call puts
	popa
	ret

HexStrBuff db "0x0000", 0

%endif