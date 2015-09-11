[bits 32]

[global start]
[extern init]

start:
	call init
	.KillTask:
		mov al, 0
		int 0xF1
		jmp .KillTask
