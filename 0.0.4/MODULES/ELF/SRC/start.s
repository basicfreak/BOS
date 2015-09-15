[bits 32]

[global start]
[extern main]

start:
	push ebx
	call main
	.KillTask:
		mov al, 0
		int 0xF1
		jmp .KillTask
