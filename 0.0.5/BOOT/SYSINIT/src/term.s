; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                Terminal Output
;                          01/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

%include 'linker.inc'

global puts64
global puts32
global puts

bits 64
puts64:
	push rax
	push rdx
	push rbx
	push rcx
	mov al, 5
	mov edx, .RMEnt
	jmp AP_Strap
bits 16
	.RMEnt:
		call puts
		mov al, 2
		mov edx, .LMEnt
		mov ebx, 0x200000
		jmp AP_Strap
bits 64
	.LMEnt:
		pop rcx
		pop rbx
		pop rdx
		pop rax
		ret

bits 32
puts32:
	pusha
	mov al, 1
	mov edx, .RMEnt
	jmp AP_Strap
bits 16
	.RMEnt:
		call puts
		mov al, 0
		mov edx, .PMEnt
		jmp AP_Strap
bits 32
	.PMEnt:
		popa
		ret
bits 16
puts:
	pusha									; Save Registers
	lodsb
	mov bl, al
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
