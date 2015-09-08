;----------------------------------------------------
;                    SVGA.inc
;                   2015-07-09
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]

;----------------------------------------------------
;                    VBEFindMode
;INPUT: WORD [VBEWidth], WORD [VBEHeight] BYTE [VBEDepth]
;             OUTPUT: DX = mode | FFFF
;----------------------------------------------------
VBEFindMode:
	push bx
	push si
	mov bx, 0
	mov bl, BYTE [VBELFB]
	mov ax, 0x4F00
	mov di, 0x2000
	int 0x10
	push ax
	mov dx, 0xFFFF
	cmp al, 0x4F
	jne .VBEDone
	pop ax
	mov si, WORD [es:di + 0x0E]
	mov ax, WORD [es:di + 0x10]
	push ax
	.modeLoop:
		pop ax
		push es
		mov es, ax
		mov dx, word [ES:SI]
		pop es
		push ax
		cmp dx, 0FFFFh
		je .VBEDone
		add si, 2
		mov CX, DX
		mov AX, 4F01h
		mov DI, 0x2100
		int 10h
		;; Make sure the function call is supported
		cmp AL, 4Fh
		;; Fail
		jne ERROR
		cmp AH, 00h
		jne .modeLoop
		mov ax, word[es:di]

		test bl, bl
		jz .noLFB1
		and ax, 0x91 ; use 0x11 for NON-LFB
		cmp ax, 0x91 ; use 0x91 for LFB
		jne .modeLoop
		.noLFB1:
			and ax, 0x11
			cmp ax, 0x11
			jne .modeLoop
		mov ax, WORD [es:di + 0x12]
		cmp ax, WORD [VBEWidth]
		jne .modeLoop
		mov ax, WORD [es:di + 0x14]
		cmp ax, WORD [VBEHeight]
		jne .modeLoop
		mov al, BYTE [es:di + 0x19]
		cmp al, BYTE [VBEDepth]
		jne .modeLoop
; Uncomment for LFB
	test bl, bl
	jz .VBEDone
	mov eax, DWORD [es:di + 0x28]
	mov DWORD [FrameBuffer], eax
	or dx, 0x4000
	.VBEDone:
		mov di, 0x2E00
		mov DWORD [es:di + 28], eax
		mov eax, DWORD 0xDE00
		mov DWORD [es:di + 7], eax
		mov eax, DWORD 0xDF00
		mov DWORD [es:di + 11], eax
		pop ax
		pop si
		pop bx
	ret

;----------------------------------------------------
;                    VBESetMode
;                 INPUT: DX = mode
;     WARNING: THIS DOES NOT TEST COMPATIBILITY
;                 OUTPUT: NONE!!!!!
;----------------------------------------------------
VBESetMode:
	pusha
	mov ax, 0x4F02
	mov bx, dx
	int 0x10
	popa
	ret

;----------------------------------------------------
;                    S3LFBEnable
;       INPUT: WORD [VBEWidth] BYTE [VBEDepth]
;     WARNING: THIS DOES NOT TEST COMPATIBILITY
;                 OUTPUT: NONE!!!!!
;----------------------------------------------------
S3LFBEnable:
	pusha
	mov dx, 0x3C4
	mov al, 0x08
	out dx, al
	inc dx
	mov al, 0x06
	out dx, al
	mov dx, 0x3D4
	mov al, 0x38
	out dx, al
	inc dx
	mov al, 0x48
	out dx, al
	dec dx
	mov al, 0x39
	out dx, al
	inc dx
	mov al, 0xA5
	out dx, al
	dec dx
	mov al, 0x33
	out dx, al
	inc dx
	mov al, 0x48
	out dx, al
	in al, dx
	dec dx
	push ax
	mov al, 0x33
	out dx, al
	pop ax
	inc dx
	and al, 0xAD
	out dx, al
	
; Uncomment for Hardware Accelerator
;	mov dx, 0x4AE8
;	mov ax, 1
;	out dx, ax

	mov dx, 0x3D4
	mov al, 0x31
	out dx, al
	inc dx
	in al, dx
	push ax
	dec dx
	mov al, 0x31
	out dx, al
	pop ax
	or al, 9
	inc dx
	out dx, al
	dec dx
	mov al, 0x58
	out dx, al
	inc dx
	in al, dx
	push ax
	dec dx
	mov al, 0x58
	out dx, al
	pop ax
	inc dx
	or al, 0x13
	out dx, al
	dec dx
	mov al, 0x13
	out dx, al
	inc dx

	mov ax, WORD [VBEWidth]
	xor bx, bx
	mov bl, BYTE [VBEDepth]
	mul bx
	shr ax, 6

;	mov ax, 1600
;	shr ax, 3



	out dx, ax
	mov eax, DWORD 0xF8000000
	mov DWORD [FrameBuffer], eax
	mov di, 0x2E00
	mov DWORD [es:di + 28], eax
	popa
	ret


GetPModeInterface:
	pusha
	push es

	mov ax, 0x4F0A
	xor bx, bx
	int 0x10

	cmp al, 0x4F
	jne ERROR

	test ah, ah
	jnz ERROR

	
	mov eax, DWORD 0
	mov ebx, DWORD 0
	mov bx, di
	mov ax, es
	dw 0x6766
		shl eax, 4
	dw 0x6766
		add eax, ebx

	pop es
	mov di, 0x2E00
	mov DWORD [es:di + 40], eax

	popa
	ret


VBEWidth dw 0
VBEHeight dw 0
VBEDepth db 0
VBELFB db 0
FrameBuffer dd 0