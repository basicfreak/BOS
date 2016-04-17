; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0006
;                                 Builder  INIT
;                          13/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 64
default rel

global _init

%include 'AP_Strap.inc'
%include 'PMM.inc'
%include 'VMM.inc'

MINIMUM_RAM		equ 0x2000000			; 32 MB Minimum

_init:
	mov [Pointers.CPUDATA], rbx			; CPUID Data Pointer
	mov [Pointers.FILE_IO], rcx			; FILE_IO Pointer
	mov [Pointers.PUTS], rdx			; PUTS Pointer
	mov rsi, MSG.PMM
	call puts
	call _PMM_init						; Initialize PMM
	jc .Fail
	mov [Pointers.Total_RAM], rax		; Total_RAM	Pointer
	mov [Pointers.Used_RAM], rbx		; Used_RAM	Pointer
	mov [Pointers.Free_RAM], rcx		; Free_RAM	Pointer
	mov rsi, MSG.Done
	call puts
	mov rax, [rax]						; RAX = Total_RAM
	cmp rax, MINIMUM_RAM				; Do we have enough RAM?
	jb .Error							; If Not Error!
	mov rsi, MSG.VMM
	call puts
	call _VMM_init						; Initialize VMM
	jc .Fail
	mov rsi, MSG.Done
	call puts
	mov rsi, MSG.IDT
	call puts
; Now maybe we can do something here... Or not, I'm still trying to work out how
; this ELF will work, it cannot be default for my design ;)
	mov rsi, FILES.IDT
	mov rdi, 0xFFFFFFFFC0200000
	call FILE_IO
	jc .Fail
	mov rsi, MSG.Done
	call puts
	.Error:
		ret
	.Fail:
		mov rsi, MSG.Fail
		call puts
		ret


puts:
	push rdi
	push rsi
	push rax
	mov rdi, 0x580
	.CopyToLowMem:
		lodsb
		stosb
		test al, al
		jnz .CopyToLowMem
	mov rsi, 0x580
	call [Pointers.PUTS]
	pop rax
	pop rsi
	pop rdi
	ret

FILE_IO:
	push rsi
	push rdi
	mov rdi, 0x580
	.CopyToLowMem:
		lodsb
		stosb
		test al, al
		jnz .CopyToLowMem
	mov rsi, 0x580
	pop rdi
	xor rax, rax
	call [Pointers.FILE_IO]
	pop rsi
	ret

section .data

Pointers:
	.CPUDATA		dq 0
	.FILE_IO		dq 0
	.PUTS			dq 0
	.Total_RAM		dq 0
	.Used_RAM		dq 0
	.Free_RAM		dq 0

section .rodata

MSG:
	.PMM			db 0x7, "Initilizing Physical Memory   ", 0
	.VMM			db 0x7, "Initilizing Virtual Memory    ", 0
	.IDT			db 0x7, "Loading Interrupt Handler     ", 0
	.IDIinst		db 0x7, "Installing Interrupt Handler  ", 0
	.Done			db 0x2, "                     [ DONE! ]", 0xA, 0xD, 0
	.Fail			db 0x4, "                     [ FAIL! ]", 0xA, 0xD, 0

FILES:
	.IDT			db "/boot/x64/idt.elf", 0
