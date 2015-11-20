%ifndef __MEMINF_INC__INCLUDED__
%define __MEMINF_INC__INCLUDED__

[bits 16]

init_MEMINF:
	call memmap
	jc ERROR
	call MemSize
	ret

;----------------------------------------------------
;                       memmap
;                    INPUT: NONE
;          OUTPUT: 0xE000 - 0xE3FF Mem Map,
;              WORD mmap_ents = entries
;----------------------------------------------------
memmap:
	pusha
	mov di, 0x7000
	xor ebx, ebx
	xor bp, bp
	mov edx, DWORD 'PAMS'
	mov eax, DWORD 0xE820
	mov ecx, DWORD 24
	mov [es:di + 20], DWORD 1 ;FORCE ACPI 3.x
	int 0x15
	jc .failed
	mov edx, DWORD 'PAMS'
	cmp eax, edx
	jne .failed
	test ebx, ebx
	jz .failed
	jmp .loop
	.e820loop:
		mov eax, DWORD 0xE820
		mov DWORD [es:di + 20], DWORD 1
		mov ecx, DWORD 24
		int 0x15
		jc .failed
		mov edx, DWORD 'PAMS'
	.loop:
		jcxz .skip
		cmp cl, 20
		jbe .notext
		test BYTE [es:di + 20], 1
		je .skip
	.notext:
		mov ecx, DWORD [es:di + 8]
		or ecx, DWORD [es:di + 12]
		jz .skip
		inc bp
		add di, 24
	.skip:
		test ebx, ebx
		jnz .e820loop
	.finished:
		mov WORD [BOOTINF.MMapEnts], bp
		mov DWORD [BOOTINF.MMapAddr], 0x7000
		clc
		popa
		ret
	.failed:
		stc
		popa
		ret

MemSize:
	pusha
	xor cx, cx
	mov ax, 0xE801
	int 0x15
	jc ERROR
	cmp ah, 0x86		;Unsupported?
	je ERROR
	cmp ah, 0x80		;Invalid?
	je ERROR
	jcxz .axisvalid
	mov ax, cx
	mov bx, dx
	.axisvalid:
		; CONTIGUOUS ONLY!
		; AX = KBs 1MB to 16MB
		; BX = 64KBs 16MB to 4GB
		mov WORD [BOOTINF.Mem_High], ax
		movzx edx, bx
		shl edx, 6
		add DWORD [BOOTINF.Mem_High], edx

	mov ax, 0
	int 0x12
	jc ERROR
	test ax, ax
	jz ERROR
	mov WORD [BOOTINF.Mem_Low], ax
	popa
	ret

%endif
