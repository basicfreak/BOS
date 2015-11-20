%ifndef __ELF_INC__INCLUDED__
%define __ELF_INC__INCLUDED__

[bits 16]

; input esi = location in memory
; output eax = Memory Start
; output ecx = Memory Size
; output edx = Entry Point
ProcessElf:
	pushad
	push esi
	mov EDI, DWORD 0x2000
	mov ECX, DWORD 0x2000
	call memcpy
	mov EDI, DWORD 0x2000
	pop esi
	mov eax, DWORD [EDI + 24] ; entry
	mov DWORD [ElfHead.entry], eax
	mov DWORD [KernelInfo.Addr], eax
	mov DWORD [KernelInfo.End], eax
	mov eax, DWORD [EDI + 28] ; phoff
	mov DWORD [ElfHead.phoff], eax
	mov ax, WORD [EDI + 42] ; phentsize
	mov WORD [ElfHead.phsize], ax
	mov ax, WORD [EDI + 44] ; phnum
	mov WORD [ElfHead.phnum], ax
	add EDI, DWORD [ElfHead.phoff]
	mov cx, ax ; it is in ax so lets save a memory access
	.ProgramHeaderLoop:
		test DWORD [EDI + 20], 0xFFFFFFFF ; memsize
		jz .PHLcont
		pushad
			add esi, DWORD [EDI + 4] ; offset
			mov ecx, DWORD [EDI + 16] ; fsize
			mov eax, DWORD [EDI + 12] ; paddr
			mov edi, eax
			call memcpy
		popad
		mov eax, DWORD [EDI + 20] ; memsize
		add DWORD [KernelInfo.Size], eax
		add DWORD [KernelInfo.End], eax
		sub eax, DWORD [EDI + 16] ; fsize
		test eax, eax
		jz .PHLcont
		;   INPUT: EDI - destination ECX - count AL - value
		pushad
			mov ecx, eax
			mov eax, DWORD [EDI + 16] ; fsize
			add eax, DWORD [EDI + 12] ; paddr
			mov edi, eax
			xor ax, ax
			call memset
		popad
		.PHLcont:
			movzx eax, WORD [ElfHead.phsize]
			add edi, eax
			dec cx
			jnz .ProgramHeaderLoop
	.PHLdone:
		popad
		ret

KernelInfo:
	.Addr dd 0
	.Size dd 0
	.End dd 0

ElfHead:
	.entry dd 0
	.phoff dd 0
	.phsize dw 0
	.phnum dw 0

%endif
