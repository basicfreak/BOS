; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                             System Initialization
;                          01/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This is mostly test code ATM

bits 16

%include 'linker.inc'
%include 'data/rodata.inc'
%include 'init/common.inc'
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
  	call init_16
    mov si, MSG.ProcessorMode
    call puts

    mov eax, [BSP.ExtendedInfo]
    bt edx, 29
    jnc .PMode

    .LMode:
    	mov si, MSG.Ok
    	call puts
    	mov BYTE [AMD64], 0xFF
    	jmp .Continue

    .PMode:
    	mov si, MSG.No
    	call puts

    .Continue:
 		mov si, MSG.BootDevice
		call puts
		mov al, BYTE [bp + VAR.bootInfo]
		bt ax, 0
		jc .FatDisk
		bt ax, 1
		jc .ExtDisk
		bt ax, 3
		jc .TftpPxe
		jmp ERROR

	.FatDisk:
		clc
		mov di, 0xA000
		mov cx, 3
		mov si, Files.FATDisk
		call BS_ReadFile
		jc .Fail
		jmp .GoToPM
	.ExtDisk:
		clc
		mov di, 0xA000
		mov si, Files.ExtDisk
		call BS_ReadFile
		jc .Fail
		jmp .GoToPM
	.TftpPxe:
		clc
		mov di, 0xA000
		mov si, Files.TFTPPXE
		call BS_ReadFile
		jc .Fail
	.GoToPM:
		mov si, MSG.Done
		call puts
		mov si, MSG.PM32
		call puts
		mov al, 0
		mov edx, PM_Entry
		jmp AP_Strap ; 0x8800


    .Fail:
    	mov si, MSG.Fail
    	call puts
    	jmp ERROR.halt
ERROR:
	mov si, MSG.Error
	call puts
	.halt:
		hlt
		jmp .halt

bits 32
PM_Entry:
	mov si, MSG.Done
	call puts32
	cmp BYTE [AMD64], 0
	je .Build86

	mov si, MSG.PDIR
	call puts32

	mov eax, 0x201000
	mov edi, 0x200000
	or eax, 0x0B
	stosd
	add eax, 0x1000
	mov edi, 0x201000
	stosd
	mov ecx, 512
	mov edi, 0x202000
	mov eax, 0x8B
	.PSELoop:
		stosd
		add eax, 0x200000
		add edi, 4
		loop .PSELoop
	mov eax, 0x200000
	mov ecx, eax
	add ecx, 3
	mov DWORD [eax + 0xFF8], ecx

	mov si, MSG.Done
	call puts32
	mov si, MSG.LOADBUILD
	call puts32

	mov esi, Files.BUILD64
	xor eax, eax
	call FILE_IO
	jc .Fail
	mov edi, 0x100000
	mov eax, 1
	call FILE_IO
	jc .Fail
	mov si, MSG.Done
	call puts32
xchg bx, bx
	mov edx, 0x100000
	mov al, 3
	mov ebx, 0x200000
	jmp AP_Strap ; 0x8800

	.Build86:
		mov si, MSG.LOADBUILD
		call puts32
		mov esi, Files.BUILD86
		xor eax, eax
		call FILE_IO
		jc .Fail
		mov edi, 0x100000
		mov eax, 1
		call FILE_IO
		jc .Fail
		mov si, MSG.Done
		call puts32
xchg bx, bx
		jmp 0x100000

	.Fail:
		mov si, MSG.Fail
		call puts32
		mov edx, ERROR.halt
		mov al, 1
		jmp AP_Strap ; 0x8800

section .data

AMD64 db 0
