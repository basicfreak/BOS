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
	mov edi, [esp + 12]
	mov esi, [esp + 16]
	mov ecx, [esp + 20]
	rep movsb
	pop edi
	pop esi
	ret

memcpyw:
	push esi
	push edi
	mov edi, [esp + 12]
	mov esi, [esp + 16]
	mov ecx, [esp + 20]
	rep movsw
	pop edi
	pop esi
	ret

memcpyd:
	push esi
	push edi
	mov edi, [ebp + 12]
	mov esi, [ebp + 16]
	mov ecx, [ebp + 20]
	rep movsd
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
