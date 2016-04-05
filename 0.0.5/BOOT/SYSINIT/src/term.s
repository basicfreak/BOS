; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                Terminal Output
;                          01/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------


; Yes, I notice that I am using BIOS even in Long Mode...
; It is only used for this stage, and maybe the very start of the next.

%include 'linker.inc'

global puts64
global puts32
global puts

bits 64
puts64:
	push rax								; Save Registers
	push rdx
	push rbx
	push rcx
	push rsi
	push rdi
	mov al, 5								; LM to RM
	mov edx, .RMEnt
	jmp AP_Strap
bits 16
	.RMEnt:
		call puts							; Call puts
		mov al, 2							; RM to LM
		mov edx, .LMEnt
		mov ebx, 0x200000
		jmp AP_Strap
bits 64
	.LMEnt:
		pop rdi								; Restore Registers
		pop rsi
		pop rcx
		pop rbx
		pop rdx
		pop rax
		ret									; Return

bits 32
puts32:
	pusha									; Save Registers
	mov al, 1								; PM to RM
	mov edx, .RMEnt
	jmp AP_Strap
bits 16
	.RMEnt:
		call puts							; Call puts
		xor al, al							; RM to PM
		xor ebx, ebx
		mov edx, .PMEnt
		jmp AP_Strap
bits 32
	.PMEnt:
		popa								; Restore Registers
		ret									; Return
bits 16
puts:
	pusha									; Save Registers
	lodsb									; Load "Color Byte"
	mov bl, al								; BL = "Color Byte"
	.StrLoop:
		lodsb								; mov al, BYTE [ds:si]; inc si
		test al, al							; test al for 0
		jz .Done							; Check for Terminator (0)
		mov ah, 0Eh							; Print Char To Screen Function
		int 10h								; Call BIOS
		jmp .StrLoop						; Loop
	.Done:
		popa								; Restore Registers
		ret									; Return
