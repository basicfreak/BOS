[bits 32]

[global start]							; LINKER CALLS START
[global enable_paging]
[global disable_paging]
[global sys_stack]

start:										; Start point of KERNEL
	mov esp, sys_stack     			    ; This points the stack to our new stack area
	jmp startsub

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
    mov ax, 0x23    ; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23       ; SS, notice it uses same selector as above
    push esp        ; ESP
    pushfd          ; EFLAGS

    pop eax
    or eax, 0x200   ; enable IF in EFLAGS
    push eax

    push 0x1b       ; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
    lea eax, [a]    ; EIP first
    push eax
    iretd
    a:
        add esp, 4 ; fix stack
ret


[extern kmain]

;
;	Landing point of kernel sub jump
;
startsub:									; Sub function from start
 ;   push eax
    push ebx
	call kmain							    ; Call kmain() from C code

	cli										; Clear Interrupts
	hlt										; Halt System
	jmp $									; Infinite Loop
	

SECTION .bss
    resb 8192               ; This reserves 8KBytes of memory here
sys_stack:
