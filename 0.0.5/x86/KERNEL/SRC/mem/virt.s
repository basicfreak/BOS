%include "./include/bos/bootinf.inc"
%include "./include/mem/phys.inc"

[global _VMM_init]
[global _VMM_map]
[global _VMM_umap]




SECTION .text ; Code area, Only Executable code please
align 0x00001000

[bits 32]

_VMM_init:
	ret

_VMM_map:
	ret

_VMM_umap:
	ret
