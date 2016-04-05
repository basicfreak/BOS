; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                         System Initialization Common
;                          03/04/2016 - Brian T Hoover
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
	mov si, MSG.BootDevice
	call puts

	mov cx, 3
	mov di, 0xA000						; Driver Address = 0:A000
	mov al, BYTE [bp + VAR.bootInfo]	; Determine Boot Device
	bt ax, 0
	jc .FatDisk
	bt ax, 1
	jc .ExtDisk
	bt ax, 3
	jc .TftpPxe
	jmp ERROR
	.FatDisk:
		mov si, Files.FATDisk
		jmp .ReadDiskDriver
	.ExtDisk:
		mov si, Files.ExtDisk
		jmp .ReadDiskDriver
	.TftpPxe:
		mov si, Files.TFTPPXE
	.ReadDiskDriver:
		call BS_ReadFile
		jc ERROR
	mov si, MSG.Done
	call puts

	mov si, MSG.PM32
	call puts
	xor al, al
	mov edx, init_32
	jmp AP_Strap

bits 32

init_32:
	mov si, MSG.Done
	call puts32
	ret
