[bits 16]
[org 0x7E00]

start:
	mov eax, DWORD [0x7C1C]
	mov DWORD [PartitionOffset], eax
	mov BYTE [bootDrive], dl
	mov BYTE [BOOTINF.BootDevice], dl ; PXE should be 0x70
	mov si, DONEMSG
	call puts
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

%include "./include/stdio/hdd.inc"
%include "./include/fat/basic_fat.inc"
%include "./include/fat/ext_fat.inc"
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

BOOTCFG db "/boot/boot.cfg", 0

WAITMSG db "...", 0
DONEMSG db ".DONE!"
NEWLINE db 13, 10, 0
ERRORMSG db "ERROR!", 0

PartitionOffset dd 0
bootDrive db 0
