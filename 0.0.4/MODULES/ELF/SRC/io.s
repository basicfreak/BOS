[bits 32]

[global outb]
[global inb]
[global outw]
[global inw]
[global outd]
[global ind]

outb:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, al
	ret

inb:
	xor eax, eax
	mov edx, [esp + 4]
	in al, dx
	ret

outw:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, ax
	ret

inw:
	xor eax, eax
	mov edx, [esp + 4]
	in ax, dx
	ret


outd:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, eax
	ret

ind:
	mov edx, [esp + 4]
	in eax, dx
	ret

