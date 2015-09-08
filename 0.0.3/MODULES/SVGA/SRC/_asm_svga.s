[bits 32]
[extern SecondBuffer]
[global refreshScreen]
[global VBE_LFB]
[global VBE_LFB1]
[global VBE_FB]
[global VBE_FBSize]
[global VBE_FUNCTION]
[global VBE_PAGE0]
[global VBE_PAGE1]
[global FlipPage]
[extern Height]
[extern Width]
[global EnableSSE]

refreshScreenold:
	pusha
	mov edx, 0x3DA
	mov ecx, DWORD [VBE_FBSize]
	shr ecx, 2 ; Divide By 4 (4 Bytes per DWORD)
	mov esi, DWORD [VBE_FB]
	mov edi, DWORD [VBE_LFB]
	.WaitVSyncEnd:
		in al, dx
		cmp al, 8
		je .WaitVSyncEnd
	.WaitVSyncStart:
		in al, dx
		cmp al, 8
		jne .WaitVSyncStart
	.UpdateLFB:
		xor edx, edx
		;Move doubleword at address DS:(E)SI to address	ES:(E)DI ECX TIMES
		rep movsd
	popa
	ret

FlipPage:
	pusha
	
	mov ecx, DWORD [VBE_FBSize]
	shr ecx, 2

	mov esi, DWORD [VBE_FB]

	mov eax, DWORD [SecondBuffer]
	test eax, eax
	jz .NotSecond

	mov edi, DWORD [VBE_LFB]
	mov dx, 0
	mov cx, 0
	mov DWORD [SecondBuffer], 0
	jmp .copy

	.NotSecond:
		mov edi, DWORD [VBE_LFB1]
		mov dx, WORD [Height]
		mov cx, WORD [Width]
		mov DWORD [SecondBuffer], 1
	.copy:
;		rep movsd

	;Now Lets Page Flip
	xor bx, bx
	;mov cx, 0
	mov ax, 0x4F07
	;mov bl, 0x80
	xchg bx, bx
	call DWORD [VBE_FUNCTION]
	test ah, ah
	jnz .hltLoop

	popa
	ret

.hltLoop:
	hlt
	jmp .hltLoop


refreshScreen:
	pusha

	mov ecx, DWORD [VBE_FBSize]
	shr ecx, 7
	mov esi, [VBE_FB]
	mov edi, [VBE_LFB]
	;VSync cannot be trusted on all devices!
	;mov edx, 0x3DA
	;.WaitVSyncEnd:
	;	in al, dx
	;	cmp al, 8
	;	je .WaitVSyncEnd
	;.WaitVSyncStart:
	;	in al, dx
	;	cmp al, 8
	;	jne .WaitVSyncStart
	.copyloop:
		MOVAPS xmm0, [es:esi]
		MOVAPS xmm1, [es:esi + 0x10]
		MOVAPS xmm2, [es:esi + 0x20]
		MOVAPS xmm3, [es:esi + 0x30]
		MOVAPS xmm4, [es:esi + 0x40]
		MOVAPS xmm5, [es:esi + 0x50]
		MOVAPS xmm6, [es:esi + 0x60]
		MOVAPS xmm7, [es:esi + 0x70]
		
		MOVAPS [es:edi], xmm0
		MOVAPS [es:edi + 0x10], xmm1
		MOVAPS [es:edi + 0x20], xmm2
		MOVAPS [es:edi + 0x30], xmm3
		MOVAPS [es:edi + 0x40], xmm4
		MOVAPS [es:edi + 0x50], xmm5
		MOVAPS [es:edi + 0x60], xmm6
		MOVAPS [es:edi + 0x70], xmm7
		
		add esi, 0x80
		add edi, 0x80
		dec ecx
		jnz .copyloop

	popa
	ret

VBE_LFB dd 0
VBE_LFB1 dd 0
VBE_FB dd 0
VBE_FBSize dd 0

VBE_PAGE0 dw 0
VBE_PAGE1 dw 0

VBE_FUNCTION dd 0