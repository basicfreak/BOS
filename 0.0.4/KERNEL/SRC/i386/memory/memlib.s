[bits 32]
[global memcpy]
[global memcpyw]
[global memcpyd]
[global memset]
[global memsetw]
[global memsetd]

memcpy:
	push esi
	push edi
	push ecx
	mov edi, [esp + 16]
	mov esi, [esp + 20]
	mov ecx, [esp + 24]
	rep movsb
	pop ecx
	pop edi
	pop esi
	ret

memcpyw:
	push esi
	push edi
	push ecx
	mov edi, [esp + 16]
	mov esi, [esp + 20]
	mov ecx, [esp + 24]
	rep movsw
	pop ecx
	pop edi
	pop esi
	ret

memcpyd:
	push esi
	push edi
	push ecx
	mov edi, [ebp + 16]
	mov esi, [ebp + 20]
	mov ecx, [ebp + 24]
	rep movsd
	pop ecx
	pop edi
	pop esi
	ret

;ebp, ebx, esi, edi are "callee save" registers

memset:
	mov eax, [esp + 4]
	mov edx, [esp + 8]
	mov ecx, [esp + 12]
	.loop:
		mov BYTE [eax], dl
		inc eax
		dec ecx
		jnz .loop
	ret

memsetw:
	mov eax, [esp + 4]
	mov edx, [esp + 8]
	mov ecx, [esp + 12]
	.loop:
		mov WORD [eax], dx
		add eax, 2
		dec ecx
		jnz .loop
	ret

memsetd:
	mov eax, [esp + 4]
	mov edx, [esp + 8]
	mov ecx, [esp + 12]
	.loop:
		mov DWORD [eax], edx
		add eax, 4
		dec ecx
		jnz .loop
	ret
