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
	mov eax, PM_Entry
	push eax
	jmp init_16

ERROR:
	mov si, MSG.Error
	call puts
	.halt:
		hlt
		jmp .halt
bits 32

PM_Entry:
	mov si, MSG.ProcessorMode
    call puts32
    mov eax, [BSP.ExtendedInfo]
    bt eax, 29
    jnc .PMode

    .LMode:
    	mov si, MSG.Ok
    	call puts32
    	push 0
    	call init_x64
		mov al, 5
		mov edx, ERROR
		jmp AP_Strap

    .PMode:
		mov si, MSG.No
		call puts32
		call init_x86
		mov al, 1
		mov edx, ERROR
		jmp AP_Strap
