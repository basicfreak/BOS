[bits 32]

[global start]							; LINKER CALLS START

start:										; Start point of KERNEL
	mov		esp, sys_stack     			    ; This points the stack to our new stack area
	jmp		startsub						; Only have 4 bytes lets move to sub function

[extern kmain]

;
;	Landing point of kernel sub jump
;
startsub:									; Sub function from start
;    push eax
xchg bx, bx
    push ebx
	call kmain							    ; Call kmain() from C code

	cli										; Clear Interrupts
	hlt										; Halt System
	jmp $									; Infinite Loop
	

SECTION .bss
    resb 8192               ; This reserves 8KBytes of memory here
sys_stack:
