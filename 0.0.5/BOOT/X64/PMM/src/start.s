bits 64

extern FILE_IO
extern AP_Strap
extern end

global start

start:
	xchg bx, bx
	mov rax, end
	test rax, 0xFFF
	jz .EndIsAligned
	add rax, 0x1000
	and rax, 0xFFFFFFFFFFFFF000
	.EndIsAligned:


section .rodata

BOS_OBJECT:
	.PMM	db	"/boot/x64/pmm.bos", 0
	.VMM	db	"/boot/x64/vmm.bos", 0
