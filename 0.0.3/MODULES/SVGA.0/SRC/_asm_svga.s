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

refreshScreen:
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
	mov dx, WORD [VBE_PAGE0]
	mov DWORD [SecondBuffer], 0
	jmp .copy

	.NotSecond:
		mov edi, DWORD [VBE_LFB1]
		mov dx, WORD [VBE_PAGE1]
		mov DWORD [SecondBuffer], 1
	.copy:
		rep movsd

	;Now Lets Page Flip
	xor bx, bx
	call DWORD [VBE_FUNCTION]
	test ax, ax
	jnz .hltLoop

	popa
	ret

.hltLoop:
	hlt
	jmp .hltLoop


VBE_LFB dd 0
VBE_LFB1 dd 0
VBE_FB dd 0
VBE_FBSize dd 0

VBE_PAGE0 dw 0
VBE_PAGE1 dw 0

VBE_FUNCTION dd 0