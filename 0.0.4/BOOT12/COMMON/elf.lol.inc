;----------------------------------------------------
;                    elf.inc
;                   2015-07-15
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

;ESI = Location In Memory
[bits 16]
ProcessElf:
	pusha
	db 0x66, 0x67
		push esi
;	db 0x66, 0x67
		mov EDI, DWORD 0x7E00
;	db 0x66, 0x67
		mov ECX, DWORD 0x200
	call memcpy
	;db 0x66, 0x67
		mov EDI, DWORD 0x7E00
	db 0x66, 0x67
		pop esi
	;db 0x66, 0x67
		mov eax, DWORD [EDI + 24]
	;db 0x66, 0x67
		mov DWORD [KernelAddr], eax
		mov DWORD [KernelEnd], eax
		mov DWORD [KernelSize], DWORD 0
xchg bx, bx
;42 ent size
;44 ent count
	db 0x66, 0x67
		xor ebx, ebx
	mov dx, WORD [EDI+44] ; Program Header Entry Count
	mov bx, WORD [EDI+42] ; Program Entry Size
	

	mov eax, DWORD [EDI + 28]
	db 0x66, 0x67
		add edi, eax

	.pheadLoop:
		add ESI, DWORD [EDI + 4]
		mov ECX, DWORD [EDI + 16]
		db 0x66, 0x67
			push ecx
		db 0x66, 0x67
			shr ecx, 2
		mov eax, DWORD [EDI + 20]
		add DWORD [KernelSize], eax
		mov eax, DWORD [edi + 12]
		db 0x66, 0x67
			push edi
		db 0x66, 0x67
			mov edi, eax
		call memcpy
		mov eax, DWORD [KernelEnd]
		add eax, DWORD [KernelSize]
		db 0x66, 0x67
		dec eax
		mov DWORD [KernelEnd], eax
		db 0x66, 0x67
		inc eax
		db 0x66, 0x67
			pop ecx
		db 0x66, 0x67
			sub eax, ecx
		db 0x66, 0x67
			mov ecx, eax
		sub ecx, DWORD [KernelAddr]
		mov al, 0
		call memset
		db 0x66, 0x67
			pop edi
		db 0x66, 0x67
			dec edx
		db 0x66, 0x67
			test edx, edx
		jz .pheadDone
		db 0x66, 0x67
			and ebx, 0xFFFF
		db 0x66, 0x67
			add edi, ebx
		jmp .pheadLoop
	.pheadDone:

	popa
	ret