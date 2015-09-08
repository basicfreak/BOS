;./DRIVERSRC/SYSTEM/CPU/IRQ.ASM

[bits 32]

[extern _IRQ_HANDLER]

[global _IRQ0]
[global _IRQ1]
[global _IRQ2]
[global _IRQ3]
[global _IRQ4]
[global _IRQ5]
[global _IRQ6]
[global _IRQ7]
[global _IRQ8]
[global _IRQ9]
[global _IRQ10]
[global _IRQ11]
[global _IRQ12]
[global _IRQ13]
[global _IRQ14]
[global _IRQ15]

_IRQ0:
	cli
	push byte 0
	push byte 32
	jmp IRQ_COMMON
	
_IRQ1:
	cli
	push byte 0
	push byte 33
	jmp IRQ_COMMON
	
_IRQ2:
	cli
	push byte 0
	push byte 34
	jmp IRQ_COMMON
	
_IRQ3:
	cli
	push byte 0
	push byte 35
	jmp IRQ_COMMON
	
_IRQ4:
	cli
	push byte 0
	push byte 36
	jmp IRQ_COMMON
	
_IRQ5:
	cli
	push byte 0
	push byte 37
	jmp IRQ_COMMON
	
_IRQ6:
	cli
	push byte 0
	push byte 38
	jmp IRQ_COMMON
	
_IRQ7:
	cli
	push byte 0
	push byte 39
	jmp IRQ_COMMON
	
_IRQ8:
	cli
	push byte 0
	push byte 40
	jmp IRQ_COMMON
	
_IRQ9:
	cli
	push byte 0
	push byte 41
	jmp IRQ_COMMON
	
_IRQ10:
	cli
	push byte 0
	push byte 42
	jmp IRQ_COMMON
	
_IRQ11:
	cli
	push byte 0
	push byte 43
	jmp IRQ_COMMON
	
_IRQ12:
	cli
	push byte 0
	push byte 44
	jmp IRQ_COMMON
	
_IRQ13:
	cli
	push byte 0
	push byte 45
	jmp IRQ_COMMON
	
_IRQ14:
	cli
	push byte 0
	push byte 46
	jmp IRQ_COMMON
	
_IRQ15:
	cli
	push byte 0
	push byte 47
	jmp IRQ_COMMON
	
IRQ_COMMON:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, _IRQ_HANDLER
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8				; Cleans up the pushed error code and pushed ISR number
	iret
	