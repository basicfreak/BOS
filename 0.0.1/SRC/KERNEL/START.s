[bits 32]

[global start]							; LINKER CALLS START
[global sys_stack]
start:										; Start point of KERNEL
	mov		esp, sys_stack     			; This points the stack to our new stack area
	jmp		startsub						; Only have 4 bytes lets move to sub function
	
;Lets make this GRUB compatible:
ALIGN 4
mboot:
    MULTIBOOT_PAGE_ALIGN	equ 1<<0
    MULTIBOOT_MEMORY_INFO	equ 1<<1
    MULTIBOOT_AOUT_KLUDGE	equ 1<<16
    MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    EXTERN code, bss, end
	dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    dd mboot
    dd code
    dd bss
    dd end
    dd start
;GRUB DONE.

;All external functions available from C code
[extern main]
[extern	gp]
[extern	idtp]

;All local functions available to C code
[global	gdt_flush]
[global	idt_load]

;
;	MMU/CPU Functions
;
 
[global enable_paging]
[global disable_paging]
[global pmmngr_get_PDBR]
[global pmmngr_get_CR0]
[global enter_usermode]


enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
ret

disable_paging:
	mov eax, cr0
	and eax, 0x7FFFFFFF
	mov cr0, eax
ret

pmmngr_get_PDBR:
	mov eax, cr3
ret

pmmngr_get_CR0:
	mov eax, cr0
ret

enter_usermode:
	cli
	mov ax, 0x23	; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push 0x23		; SS, notice it uses same selector as above
	push esp		; ESP
	pushfd			; EFLAGS

	pop eax
	or eax, 0x200	; enable IF in EFLAGS
	push eax

	push 0x1b		; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
	lea eax, [a]	; EIP first
	push eax
	iretd
	a:
		add esp, 4 ; fix stack
ret

;
;	Landing point of kernel sub jump
;
startsub:									; Sub function from start
	call main							; Call main() from C code

	cli										; Clear Interrupts
	hlt										; Halt System
	jmp $									; Infinite Loop
	

gdt_flush:
    lgdt [gp]        						; Load the GDT with our '_gp' which is a special pointer
    mov ax, 0x10      						; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   						; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               						; Returns back to the C code!
	
	
; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void idt_load();'
idt_load:
    lidt [idtp]
    ret

	
[extern ISR_HANDLER]

[global ISR0]
[global ISR1]
[global ISR2]
[global ISR3]
[global ISR4]
[global ISR5]
[global ISR6]
[global ISR7]
[global ISR8]
[global ISR9]
[global ISR10]
[global ISR11]
[global ISR12]
[global ISR13]
[global ISR14]
[global ISR15]
[global ISR16]
[global ISR17]
[global ISR18]
[global ISR19]
[global ISR20]
[global ISR21]
[global ISR22]
[global ISR23]
[global ISR24]
[global ISR25]
[global ISR26]
[global ISR27]
[global ISR28]
[global ISR29]
[global ISR30]
[global ISR31]

; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
;  0: Divide By Zero Exception
ISR0:
    cli
    push byte 0
    push byte 0
    jmp ISR_COMMON

;  1: Debug Exception
ISR1:
    cli
    push byte 0
    push byte 1
    jmp ISR_COMMON

;  2: Non Maskable Interrupt Exception
ISR2:
    cli
    push byte 0
    push byte 2
    jmp ISR_COMMON

;  3: Int 3 Exception
ISR3:
    cli
    push byte 0
    push byte 3
    jmp ISR_COMMON

;  4: INTO Exception
ISR4:
    cli
    push byte 0
    push byte 4
    jmp ISR_COMMON

;  5: Out of Bounds Exception
ISR5:
    cli
    push byte 0
    push byte 5
    jmp ISR_COMMON

;  6: Invalid Opcode Exception
ISR6:
    cli
    push byte 0
    push byte 6
    jmp ISR_COMMON

;  7: Coprocessor Not Available Exception
ISR7:
    cli
    push byte 0
    push byte 7
    jmp ISR_COMMON

;  8: Double Fault Exception (With Error Code!)
ISR8:
    cli
    push byte 8
    jmp ISR_COMMON

;  9: Coprocessor Segment Overrun Exception
ISR9:
    cli
    push byte 0
    push byte 9
    jmp ISR_COMMON

; 10: Bad TSS Exception (With Error Code!)
ISR10:
    cli
    push byte 10
    jmp ISR_COMMON

; 11: Segment Not Present Exception (With Error Code!)
ISR11:
    cli
    push byte 11
    jmp ISR_COMMON

; 12: Stack Fault Exception (With Error Code!)
ISR12:
    cli
    push byte 12
    jmp ISR_COMMON

; 13: General Protection Fault Exception (With Error Code!)
ISR13:
    cli
    push byte 13
    jmp ISR_COMMON

; 14: Page Fault Exception (With Error Code!)
ISR14:
    cli
    push byte 14
    jmp ISR_COMMON

; 15: Reserved Exception
ISR15:
    cli
    push byte 0
    push byte 15
    jmp ISR_COMMON

; 16: Floating Point Exception
ISR16:
    cli
    push byte 0
    push byte 16
    jmp ISR_COMMON

; 17: Alignment Check Exception
ISR17:
    cli
    push byte 0
    push byte 17
    jmp ISR_COMMON

; 18: Machine Check Exception
ISR18:
    cli
    push byte 0
    push byte 18
    jmp ISR_COMMON

; 19: Reserved
ISR19:
    cli
    push byte 0
    push byte 19
    jmp ISR_COMMON

; 20: Reserved
ISR20:
    cli
    push byte 0
    push byte 20
    jmp ISR_COMMON

; 21: Reserved
ISR21:
    cli
    push byte 0
    push byte 21
    jmp ISR_COMMON

; 22: Reserved
ISR22:
    cli
    push byte 0
    push byte 22
    jmp ISR_COMMON

; 23: Reserved
ISR23:
    cli
    push byte 0
    push byte 23
    jmp ISR_COMMON

; 24: Reserved
ISR24:
    cli
    push byte 0
    push byte 24
    jmp ISR_COMMON

; 25: Reserved
ISR25:
    cli
    push byte 0
    push byte 25
    jmp ISR_COMMON

; 26: Reserved
ISR26:
    cli
    push byte 0
    push byte 26
    jmp ISR_COMMON

; 27: Reserved
ISR27:
    cli
    push byte 0
    push byte 27
    jmp ISR_COMMON

; 28: Reserved
ISR28:
    cli
    push byte 0
    push byte 28
    jmp ISR_COMMON

; 29: Reserved
ISR29:
    cli
    push byte 0
    push byte 29
    jmp ISR_COMMON

; 30: Reserved
ISR30:
    cli
    push byte 0
    push byte 30
    jmp ISR_COMMON

; 31: Reserved
ISR31:
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
    mov eax, ISR_HANDLER
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

[extern IRQ_HANDLER]

[global IRQ0]
[global IRQ1]
[global IRQ2]
[global IRQ3]
[global IRQ4]
[global IRQ5]
[global IRQ6]
[global IRQ7]
[global IRQ8]
[global IRQ9]
[global IRQ10]
[global IRQ11]
[global IRQ12]
[global IRQ13]
[global IRQ14]
[global IRQ15]

[extern PIT_TASK_IR]

[global _SYSTEM_IDLE_THREAD]
_SYSTEM_IDLE_THREAD:
	hlt
	jmp _SYSTEM_IDLE_THREAD

IRQ0:		; PIT This will be different than all the others for my scheduler
	cli
	; Save Registers
	pusha
	push ds
	push es
	push fs
	push gs
	mov eax, esp
	; Call Int Routine
	push eax
	mov eax, PIT_TASK_IR
	call eax
	; Send 0x20 to 0x20 EOI - Primary PIC
	mov al, 0x20
	out 0x20, al
	pop eax
	; Restore Registers
	; If task switch is done these values have been changed since push
	pop gs
	pop fs
	pop es
	pop ds
	popa
	; Return or Call Task
	iret
	
IRQ1:
	cli
	push byte 0
	push byte 33
	jmp IRQ_COMMON
	
IRQ2:
	cli
	push byte 0
	push byte 34
	jmp IRQ_COMMON
	
IRQ3:
	cli
	push byte 0
	push byte 35
	jmp IRQ_COMMON
	
IRQ4:
	cli
	push byte 0
	push byte 36
	jmp IRQ_COMMON
	
IRQ5:
	cli
	push byte 0
	push byte 37
	jmp IRQ_COMMON
	
IRQ6:
	cli
	push byte 0
	push byte 38
	jmp IRQ_COMMON
	
IRQ7:
	cli
	push byte 0
	push byte 39
	jmp IRQ_COMMON
	
IRQ8:
	cli
	push byte 0
	push byte 40
	jmp IRQ_COMMON
	
IRQ9:
	cli
	push byte 0
	push byte 41
	jmp IRQ_COMMON
	
IRQ10:
	cli
	push byte 0
	push byte 42
	jmp IRQ_COMMON
	
IRQ11:
	cli
	push byte 0
	push byte 43
	jmp IRQ_COMMON
	
IRQ12:
	cli
	push byte 0
	push byte 44
	jmp IRQ_COMMON
	
IRQ13:
	cli
	push byte 0
	push byte 45
	jmp IRQ_COMMON
	
IRQ14:
	cli
	push byte 0
	push byte 46
	jmp IRQ_COMMON
	
IRQ15:
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
	mov eax, IRQ_HANDLER
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8				; Cleans up the pushed error code and pushed ISR number
	iret


[extern INT_HANDLER]

[global INT0]
[global INT1]
[global INT2]
[global INT3]
[global INT4]
[global INT5]
[global INT6]
[global INT7]
[global INT8]
[global INT9]
[global INT10]
[global INT11]
[global INT12]
[global INT13]
[global INT14]
[global INT15]
[global INT16]
[global INT17]
[global INT18]
[global INT19]
[global INT20]
[global INT21]
[global INT22]
[global INT23]
[global INT24]
[global INT25]
[global INT26]
[global INT27]
[global INT28]
[global INT29]
[global INT30]
[global INT31]


INT0:
    cli
    push byte 0
    push byte 0
    jmp INT_COMMON

INT1:
    cli
    push byte 0
    push byte 1
    jmp INT_COMMON

INT2:
    cli
    push byte 0
    push byte 2
    jmp INT_COMMON

INT3:
    cli
    push byte 0
    push byte 3
    jmp INT_COMMON

INT4:
    cli
    push byte 0
    push byte 4
    jmp INT_COMMON

INT5:
    cli
    push byte 0
    push byte 5
    jmp INT_COMMON

INT6:
    cli
    push byte 0
    push byte 6
    jmp INT_COMMON

INT7:
    cli
    push byte 0
    push byte 7
    jmp INT_COMMON

INT8:
    cli
	push byte 0
    push byte 8
    jmp INT_COMMON

INT9:
    cli
    push byte 0
    push byte 9
    jmp INT_COMMON

INT10:
    cli
	push byte 0
    push byte 10
    jmp INT_COMMON

INT11:
    cli
	push byte 0
    push byte 11
    jmp INT_COMMON

INT12:
    cli
	push byte 0
    push byte 12
    jmp INT_COMMON

INT13:
    cli
	push byte 0
    push byte 13
    jmp INT_COMMON

INT14:
    cli
	push byte 0
    push byte 14
    jmp INT_COMMON

INT15:
    cli
    push byte 0
    push byte 15
    jmp INT_COMMON

INT16:
    cli
    push byte 0
    push byte 16
    jmp INT_COMMON

INT17:
    cli
    push byte 0
    push byte 17
    jmp INT_COMMON

INT18:
    cli
    push byte 0
    push byte 18
    jmp INT_COMMON

INT19:
    cli
    push byte 0
    push byte 19
    jmp INT_COMMON

INT20:
    cli
    push byte 0
    push byte 20
    jmp INT_COMMON

INT21:
    cli
    push byte 0
    push byte 21
    jmp INT_COMMON

INT22:
    cli
    push byte 0
    push byte 22
    jmp INT_COMMON

INT23:
    cli
    push byte 0
    push byte 23
    jmp INT_COMMON

INT24:
    cli
    push byte 0
    push byte 24
    jmp INT_COMMON

INT25:
    cli
    push byte 0
    push byte 25
    jmp INT_COMMON

INT26:
    cli
    push byte 0
    push byte 26
    jmp INT_COMMON

INT27:
    cli
    push byte 0
    push byte 27
    jmp INT_COMMON

INT28:
    cli
    push byte 0
    push byte 28
    jmp INT_COMMON

INT29:
    cli
    push byte 0
    push byte 29
    jmp INT_COMMON

INT30:
    cli
    push byte 0
    push byte 30
    jmp INT_COMMON

INT31:
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
    mov eax, INT_HANDLER
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

SECTION .bss
    resb 16384               ; This reserves 16KBytes of memory here
sys_stack:
