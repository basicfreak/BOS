%ifndef __SVGA_INC__INCLUDED__
%define __SVGA_INC__INCLUDED__

[bits 16]

FindSVGAMode:
	pusha
	; Get Mode List.
	mov ax, 0x4F00
	mov di, 0x1000
	int 0x10
	test ah, ah
	jnz ERROR
	cmp al, 0x4F
	jne ERROR
	; Check Each Mode for match.
	push fs
	mov ax, WORD [ds:di + 0x10]
	mov fs, ax
	mov si, WORD [ds:di + 0x0E]

	.ModeLoop:
		mov cx, WORD [fs:si]
		cmp cx, 0xFFFF
		je ERROR
		mov WORD [VBEMODE], cx
		add si, 2
		mov AX, 4F01h
		mov DI, VBEINFO
		int 0x10
		test ah, ah
		jnz ERROR
		cmp al, 0x4F
		jne ERROR
		mov ax, WORD [VBEINFO.attributes]
		test BYTE [REQUESTEDSVGA.LFB], 0xFF
		jz .noLFB
		and ax, 0x91
		cmp ax, 0x91
		jne .ModeLoop
		or WORD [VBEMODE], 0x4000
		.noLFB:
			and ax, 0x11
			cmp ax, 0x11
			jne .ModeLoop
		mov ax, WORD [VBEINFO.Width]
		cmp WORD [REQUESTEDSVGA.Width], ax
		jne .ModeLoop
		mov ax, WORD [VBEINFO.Height]
		cmp WORD [REQUESTEDSVGA.Height], ax
		jne .ModeLoop
		mov al, BYTE [VBEINFO.BitsPerPix]
		cmp BYTE [REQUESTEDSVGA.Depth], al
		jne .ModeLoop
	pop fs
	popa
	ret

SetSVGAMode:
	mov ax, 0x4F02
	mov bx, WORD [VBEMODE]
	int 0x10
	ret

VBEMODE dw 0

REQUESTEDSVGA:
	.Width dw 0
	.Height dw 0
	.Depth db 0
	.LFB db 0

%endif