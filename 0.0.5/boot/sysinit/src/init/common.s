; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0006
;                         System Initialization Common
;                          18/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global init_16

extern gdt
extern ERROR

section .text
bits 16

%include 'data/rodata.inc'
%include 'CPUID.inc'
%include 'A20.inc'
%include 'PIC.inc'
%include 'MMAP.inc'
%include 'term.inc'
%include 'linker.inc'

VAR:
	.bootDrive		equ -127			; BYTE
	.bootInfo		equ -126			; BitField
		; 13/42 | 13/02 | PXE | ? | TFTP | ? | ExtFS | FATFS

bootDrive			equ 0x7B81
bootInfo			equ 0x7B82

init_16:
	mov ax, 0x0012						; Set video mode to 0x0012
	int 0x10

	mov si, MSG.Identify
	call puts

	call INIT_CPUID						; Get CPUID

	jc ERROR

	mov eax, [BSP.ExtendedInfo]
    bt eax, 29							; Check for Long Mode
    jnc .PMode

; To comply with all documentation...
    mov eax, 0xEC00						; I'm unsure if this even exists
    mov ebx, 3							; This is a waste of space
    int 0x15							; Why DO I NEED this?!?
    clc									; (I'm not checking this) CF=0!
; Likely hood is I'll find the one system that will crash and burn due to not
; supporting mixed operations mode... But it's a risk I'm willing to take...
; I only got about 2.5KB for code in this file after all...

  .PMode:
	mov si, MSG.Done
	call puts
	mov si, MSG.GDT
	call puts

	lgdt [gdt]							; Install GDT

	mov si, MSG.Done
	call puts
	mov si, MSG.A20
	call puts

	call Enable_A20						; Enable A20

    jc ERROR
	mov si, MSG.Done
	call puts
	mov si, MSG.PIC
	call puts

	call INIT_PIC                       ; Remap the PIC

	jc ERROR
	mov si, MSG.Done
	call puts
	mov si, MSG.MMAP
	call puts

	call INIT_MMAP                      ; Get Memory Map (E820)

	jc ERROR
	mov si, MSG.Done
	call puts

	mov si, MSG.PM32
	call puts
	xor al, al							; RM to PM
	xor ebx, ebx
	mov edx, init_32
	jmp AP_Strap

bits 32

init_32:
	mov si, MSG.Done
	call puts32
	ret									; Return to Protected Mode Address
