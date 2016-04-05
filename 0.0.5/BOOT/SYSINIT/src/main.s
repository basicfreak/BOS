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
    jnc .PMode							; If no Long Mode, Initialize as x86

    .LMode:
    	mov si, MSG.Ok
    	call puts32
    	push 0
    	call init_x64					; Initialize x86_64
		mov al, 5
		mov edx, ERROR
		jmp AP_Strap					; Error if we returned

    .PMode:
		mov si, MSG.No
		call puts32
		call init_x86					; Initialize x86
		mov al, 1
		mov edx, ERROR
		jmp AP_Strap					; Error if we returned
