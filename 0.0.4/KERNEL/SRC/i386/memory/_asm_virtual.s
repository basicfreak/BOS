[bits 32]

[global getPageDir]
[global setPageDir]
[global getCR2]

getPageDir:
	mov eax, cr3
	ret

setPageDir:
	push eax
	mov eax, [esp + 8]
	mov cr3, eax
	pop eax
	ret

getCR2:
	mov eax, cr2
	ret