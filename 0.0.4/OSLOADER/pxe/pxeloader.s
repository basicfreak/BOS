;----------------------------------------------------
;                  pxeloader.s
;                   2015-10-20
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]

[org 0x7C00]

start:
	cli
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0xFFF0
	mov BYTE [BOOTINF.BootDevice], dl ; PXE should be 0x70
	call init_PXE
	xor ax, ax
	mov es, ax
	call init_A20
	call init_GDT
	sti
;----------------------------------------------------
;                       memset
;   INPUT: EDI - destination ECX - count AL - value
;----------------------------------------------------
	mov edi, DWORD 0x7000
	mov ecx, DWORD 0x400
	xor ax, ax
	call memset

	call init_MEMINF
	call init_Config
	call processConfig

ERROR:
	mov si, ERRORMSG
	call puts
	.halt:
		hlt
		jmp .halt

WAITMSG db "...", 0
DONEMSG db ".DONE!"
NEWLINE db 13, 10, 0
ERRORMSG db "ERROR!", 0

%include "./include/pxeapi/baseapi.inc"
%include "./include/pxeapi/tftp.inc"
%include "./include/config/config.inc"
%include "./include/stdio/basic_stdio.inc"
%include "./include/stdio/ext_stdio.inc"
%include "./include/stdio/string.inc"
%include "./include/stdio/memio.inc"
%include "./include/x86/gdt.inc"
%include "./include/x86/a20.inc"
%include "./include/elf.inc"
%include "./include/bootinf.inc"
%include "./include/x86/meminf.inc"
%include "./include/svga/svga.inc"

BOOTCFG db "pxeboot.cfg", 0
