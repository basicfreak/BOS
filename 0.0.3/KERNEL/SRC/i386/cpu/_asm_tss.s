[bits 32]

[global _TSS_flush]

_TSS_flush:
	push ax
	mov ax, 0x28
	ltr ax
	pop ax
	ret
