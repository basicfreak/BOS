;./DRIVERSRC/SYSTEM/CPU/INT.ASM

[bits 32]

[extern 	_INT_HANDLER]

[global 	_INT0]
[global 	_INT1]
[global 	_INT2]
[global 	_INT3]
[global 	_INT4]
[global 	_INT5]
[global 	_INT6]
[global 	_INT7]
[global 	_INT8]
[global 	_INT9]
[global 	_INT10]
[global 	_INT11]
[global 	_INT12]
[global 	_INT13]
[global 	_INT14]
[global 	_INT15]
[global 	_INT16]
[global 	_INT17]
[global 	_INT18]
[global 	_INT19]
[global 	_INT20]
[global 	_INT21]
[global 	_INT22]
[global 	_INT23]
[global 	_INT24]
[global 	_INT25]
[global 	_INT26]
[global 	_INT27]
[global 	_INT28]
[global 	_INT29]
[global 	_INT30]
[global 	_INT31]


_INT0:
    cli
    push byte 0
    push byte 0
    jmp INT_COMMON

_INT1:
    cli
    push byte 0
    push byte 1
    jmp INT_COMMON

_INT2:
    cli
    push byte 0
    push byte 2
    jmp INT_COMMON

_INT3:
    cli
    push byte 0
    push byte 3
    jmp INT_COMMON

_INT4:
    cli
    push byte 0
    push byte 4
    jmp INT_COMMON

_INT5:
    cli
    push byte 0
    push byte 5
    jmp INT_COMMON

_INT6:
    cli
    push byte 0
    push byte 6
    jmp INT_COMMON

_INT7:
    cli
    push byte 0
    push byte 7
    jmp INT_COMMON

_INT8:
    cli
	push byte 0
    push byte 8
    jmp INT_COMMON

_INT9:
    cli
    push byte 0
    push byte 9
    jmp INT_COMMON

_INT10:
    cli
	push byte 0
    push byte 10
    jmp INT_COMMON

_INT11:
    cli
	push byte 0
    push byte 11
    jmp INT_COMMON

_INT12:
    cli
	push byte 0
    push byte 12
    jmp INT_COMMON

_INT13:
    cli
	push byte 0
    push byte 13
    jmp INT_COMMON

_INT14:
    cli
	push byte 0
    push byte 14
    jmp INT_COMMON

_INT15:
    cli
    push byte 0
    push byte 15
    jmp INT_COMMON

_INT16:
    cli
    push byte 0
    push byte 16
    jmp INT_COMMON

_INT17:
    cli
    push byte 0
    push byte 17
    jmp INT_COMMON

_INT18:
    cli
    push byte 0
    push byte 18
    jmp INT_COMMON

_INT19:
    cli
    push byte 0
    push byte 19
    jmp INT_COMMON

_INT20:
    cli
    push byte 0
    push byte 20
    jmp INT_COMMON

_INT21:
    cli
    push byte 0
    push byte 21
    jmp INT_COMMON

_INT22:
    cli
    push byte 0
    push byte 22
    jmp INT_COMMON

_INT23:
    cli
    push byte 0
    push byte 23
    jmp INT_COMMON

_INT24:
    cli
    push byte 0
    push byte 24
    jmp INT_COMMON

_INT25:
    cli
    push byte 0
    push byte 25
    jmp INT_COMMON

_INT26:
    cli
    push byte 0
    push byte 26
    jmp INT_COMMON

_INT27:
    cli
    push byte 0
    push byte 27
    jmp INT_COMMON

_INT28:
    cli
    push byte 0
    push byte 28
    jmp INT_COMMON

_INT29:
    cli
    push byte 0
    push byte 29
    jmp INT_COMMON

_INT30:
    cli
    push byte 0
    push byte 30
    jmp INT_COMMON

_INT31:
    cli
    push byte 0
    push byte 31
    jmp INT_COMMON

INT_COMMON:
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
    mov eax, _INT_HANDLER
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
