; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                             System Initialization
;                          05/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 16

%include 'linker.inc'
%include 'data/rodata.inc'
%include 'init/common.inc'
%include 'init/x86.inc'
%include 'init/x86_64.inc'
%include 'CPUID.inc'
%include 'term.inc'

global start
global ERROR


VAR:
	.bootDrive		equ -127			; BYTE
	.bootInfo		equ -126			; BitField
		; 13/42 | 13/02 | PXE | ? | TFTP | ? | ExtFS | FATFS

bootDrive			equ 0x7B81
bootInfo			equ 0x7B82

bits 16

start:
	mov eax, PM_Entry					; Push return address (32-bit)
	push eax
	jmp init_16							; Initialize 16-Bit and 32-Bit Common

ERROR:
	mov si, MSG.Error					; Print Error Message
	call puts
	.halt:								; Lock up CPU
		hlt
		jmp .halt
bits 32

PM_Entry:
	mov si, MSG.ProcessorMode
    call puts32
    mov eax, [BSP.ExtendedInfo]
    bt eax, 29							; Check for Long Mode
    jnc PMode							; If no Long Mode, Initialize as x86

LMode:
	mov si, MSG.Ok
	call puts32

	mov si, MSG.BootDevice
	call puts32


	push 0
	push .ERROR
	push init_x64
	mov edx, ReadDiskDriver
	mov eax, 1
	mov bl, BYTE [bp + VAR.bootInfo]	; Determine Boot Device
	bt bx, 0
	jc .FatDisk
	bt bx, 1
	jc .ExtDisk
	bt bx, 3
	jc .TftpPxe
	jmp ERROR
	.FatDisk:
		mov si, Files.LM_FAT			; The Boot Device is a FAT Disk
		jmp AP_Strap
	.ExtDisk:
		mov si, Files.LM_EXT			; The Boot Device is an EXT Disk
		jmp AP_Strap
	.TftpPxe:
		mov si, Files.LM_PXE			; The Boot Device is PXE/TFTP
		jmp AP_Strap

	.ERROR:
		mov al, 5
		mov edx, ERROR
		jmp AP_Strap					; Error if we returned

PMode:
	mov si, MSG.No
	call puts32

	mov si, MSG.BootDevice
	call puts32

	push .ERROR
	push init_x86
	mov edx, ReadDiskDriver
	mov eax, 1
	mov bl, BYTE [bp + VAR.bootInfo]	; Determine Boot Device
	bt bx, 0
	jc .FatDisk
	bt bx, 1
	jc .ExtDisk
	bt bx, 3
	jc .TftpPxe
	jmp ERROR
	.FatDisk:
		mov si, Files.PM_FAT			; The Boot Device is a FAT Disk
		jmp AP_Strap
	.ExtDisk:
		mov si, Files.PM_EXT			; The Boot Device is an EXT Disk
		jmp AP_Strap
	.TftpPxe:
		mov si, Files.PM_PXE			; The Boot Device is PXE/TFTP
		jmp AP_Strap

		; call init_x86					; Initialize x86
	.ERROR:
		mov al, 1
		mov edx, ERROR
		jmp AP_Strap					; Error if we returned

bits 16
ReadDiskDriver:
	mov bp, 0x7C00
	mov cx, 3
	mov di, 0xA000						; Driver Address = 0:A000
	call BS_ReadFile
	xor al, al
	mov edx, .PMRet
	jmp AP_Strap
bits 32
	.PMRet:
		mov si, MSG.Done
		call puts32
		ret
