;./DRIVERSRC/SYSTEM/CPU/ISR.ASM
[bits 32]

[extern 	_ISR_HANDLER]

[global 	_ISR0]
[global 	_ISR1]
[global 	_ISR2]
[global 	_ISR3]
[global 	_ISR4]
[global 	_ISR5]
[global 	_ISR6]
[global 	_ISR7]
[global 	_ISR8]
[global 	_ISR9]
[global 	_ISR10]
[global 	_ISR11]
[global 	_ISR12]
[global 	_ISR13]
[global 	_ISR14]
[global 	_ISR15]
[global 	_ISR16]
[global 	_ISR17]
[global 	_ISR18]
[global 	_ISR19]
[global 	_ISR20]
[global 	_ISR21]
[global 	_ISR22]
[global 	_ISR23]
[global 	_ISR24]
[global 	_ISR25]
[global 	_ISR26]
[global 	_ISR27]
[global 	_ISR28]
[global 	_ISR29]
[global 	_ISR30]
[global 	_ISR31]

; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
;  0: Divide By Zero Exception
_ISR0:
    cli
    push byte 0
    push byte 0
    jmp ISR_COMMON

;  1: Debug Exception
_ISR1:
    cli
    push byte 0
    push byte 1
    jmp ISR_COMMON

;  2: Non Maskable Interrupt Exception
_ISR2:
    cli
    push byte 0
    push byte 2
    jmp ISR_COMMON

;  3: Int 3 Exception
_ISR3:
    cli
    push byte 0
    push byte 3
    jmp ISR_COMMON

;  4: INTO Exception
_ISR4:
    cli
    push byte 0
    push byte 4
    jmp ISR_COMMON

;  5: Out of Bounds Exception
_ISR5:
    cli
    push byte 0
    push byte 5
    jmp ISR_COMMON

;  6: Invalid Opcode Exception
_ISR6:
    cli
    push byte 0
    push byte 6
    jmp ISR_COMMON

;  7: Coprocessor Not Available Exception
_ISR7:
    cli
    push byte 0
    push byte 7
    jmp ISR_COMMON

;  8: Double Fault Exception (With Error Code!)
_ISR8:
    cli
    push byte 8
    jmp ISR_COMMON

;  9: Coprocessor Segment Overrun Exception
_ISR9:
    cli
    push byte 0
    push byte 9
    jmp ISR_COMMON

; 10: Bad TSS Exception (With Error Code!)
_ISR10:
    cli
    push byte 10
    jmp ISR_COMMON

; 11: Segment Not Present Exception (With Error Code!)
_ISR11:
    cli
    push byte 11
    jmp ISR_COMMON

; 12: Stack Fault Exception (With Error Code!)
_ISR12:
    cli
    push byte 12
    jmp ISR_COMMON

; 13: General Protection Fault Exception (With Error Code!)
_ISR13:
    cli
    push byte 13
    jmp ISR_COMMON

; 14: Page Fault Exception (With Error Code!)
_ISR14:
    cli
    push byte 14
    jmp ISR_COMMON

; 15: Reserved Exception
_ISR15:
    cli
    push byte 0
    push byte 15
    jmp ISR_COMMON

; 16: Floating Point Exception
_ISR16:
    cli
    push byte 0
    push byte 16
    jmp ISR_COMMON

; 17: Alignment Check Exception
_ISR17:
    cli
    push byte 0
    push byte 17
    jmp ISR_COMMON

; 18: Machine Check Exception
_ISR18:
    cli
    push byte 0
    push byte 18
    jmp ISR_COMMON

; 19: Reserved
_ISR19:
    cli
    push byte 0
    push byte 19
    jmp ISR_COMMON

; 20: Reserved
_ISR20:
    cli
    push byte 0
    push byte 20
    jmp ISR_COMMON

; 21: Reserved
_ISR21:
    cli
    push byte 0
    push byte 21
    jmp ISR_COMMON

; 22: Reserved
_ISR22:
    cli
    push byte 0
    push byte 22
    jmp ISR_COMMON

; 23: Reserved
_ISR23:
    cli
    push byte 0
    push byte 23
    jmp ISR_COMMON

; 24: Reserved
_ISR24:
    cli
    push byte 0
    push byte 24
    jmp ISR_COMMON

; 25: Reserved
_ISR25:
    cli
    push byte 0
    push byte 25
    jmp ISR_COMMON

; 26: Reserved
_ISR26:
    cli
    push byte 0
    push byte 26
    jmp ISR_COMMON

; 27: Reserved
_ISR27:
    cli
    push byte 0
    push byte 27
    jmp ISR_COMMON

; 28: Reserved
_ISR28:
    cli
    push byte 0
    push byte 28
    jmp ISR_COMMON

; 29: Reserved
_ISR29:
    cli
    push byte 0
    push byte 29
    jmp ISR_COMMON

; 30: Reserved
_ISR30:
    cli
    push byte 0
    push byte 30
    jmp ISR_COMMON

; 31: Reserved
_ISR31:
    cli
    push byte 0
    push byte 31
    jmp ISR_COMMON

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
ISR_COMMON:
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
    mov eax, _ISR_HANDLER
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
