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

VBEINFO:
	.attributes dw 0
	.WinAAttri db 0
	.WinBAttri db 0
	.WinGranularity dw 0
	.WinSize dw 0
	.WinAStart dw 0
	.WinBStart dw 0
	.WindowPosFunct dd 0
	.BytesPerLine dw 0
	.Width dw 0
	.Height dw 0
	.CharWidth db 0
	.CharHeight db 0
	.Planes db 0
	.BitsPerPix db 0
	.Banks db 0
	.MemModel db 0
	.BankSize db 0
	.ImgPages db 0
	.RESERVED db 0
	.RedMask db 0
	.RedField db 0
	.GreenMask db 0
	.GreenField db 0
	.BlueMask db 0
	.BlueField db 0
	.ReservedMaskSize db 0
	.ReservedMaskPos db 0
	.ColorModeInfo db 0
	.LFB dd 0
	.OffscreenMem dd 0
	.OffscreenSize dw 0
	.BytesPerScanLine dw 0
	.VBE3INFO times 76 db 0

%endif
