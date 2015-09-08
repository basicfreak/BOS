[bits 32]

[global _VMM_flush]
[global _VMM_setCR3]

_VMM_flush:
	cli
;	xchg bx, bx
	push eax
	mov eax, cr3
	mov cr3, eax
	pop eax
	sti
	ret

_VMM_setCR3:
xchg bx, bx
	cli
	push eax
	mov eax, [esp + 8]
	mov cr3, eax
	pop eax
	sti
	ret