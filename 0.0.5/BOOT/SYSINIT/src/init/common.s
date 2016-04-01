; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                         System Initialization Common
;                          01/04/2016 - Brian T Hoover
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

init_16:
	mov ax, 0x0012						; Set video mode to 0x0012
	int 0x10

	mov si, MSG.Identify
	call puts

	call INIT_CPUID						; Get CPUID

	jc ERROR
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
	ret
