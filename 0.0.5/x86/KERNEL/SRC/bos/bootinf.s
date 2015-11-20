[bits 32]

[global BOOTINF]
[global BOOTINF.BootDevice]
[global BOOTINF.MMapAddr]
[global BOOTINF.MMapEnts]
[global BOOTINF.CurVBEInfo]
[global BOOTINF.KernelStart]
[global BOOTINF.KernelEnd]
[global BOOTINF.ModList]
[global BOOTINF.ModCount]
[global BOOTINF.FrameBuffer]
[global BOOTINF.Mem_Low]
[global BOOTINF.Mem_High]
[global VBEINFO]
[global MODLIST]

SECTION .data ; Data Located in the ELF
align 0x00001000

BOOTINF:
	.BootDevice db 0
	.MMapAddr dd 0
	.MMapEnts dw 0
	.CurVBEInfo dd 0
	.KernelStart dd 0
	.KernelEnd dd 0
	.ModList dd 0
	.ModCount db 0
	.FrameBuffer dd 0
	.Mem_Low dd 0
	.Mem_High dd 0

align 0x00000100

VBEINFO times 256 db 0

MODLIST times 128 dd 0
