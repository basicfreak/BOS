%ifndef __BOOTINF_INC__INCLUDED__
%define __BOOTINF_INC__INCLUDED__

BootInf equ BOOTINF

[bits 16]

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

%endif
