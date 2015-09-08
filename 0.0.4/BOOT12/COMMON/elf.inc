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
	;db 0x66, 0x67
		mov eax, DWORD [EDI + 28]
	db 0x66, 0x67
		add edi, eax
	;db 0x66, 0x67
		add ESI, DWORD [EDI + 4]
	;db 0x66, 0x67
		mov ECX, DWORD [EDI + 16]

	db 0x66, 0x67
		push ecx
	db 0x66, 0x67
		shr ecx, 2
	mov eax, DWORD [EDI + 20]
	mov DWORD [KernelSize], eax
	;db 0x66, 0x67
		mov eax, DWORD [edi + 12]
	db 0x66, 0x67
		mov edi, eax
	call memcpy
	mov eax, DWORD [KernelAddr]
	add eax, DWORD [KernelSize]
	db 0x66, 0x67
	dec eax
	mov DWORD [KernelEnd], eax

	db 0x66, 0x67
		pop ecx
	db 0x66, 0x67
	sub eax, ecx
	db 0x66, 0x67
	mov ecx, eax
	sub ecx, DWORD [KernelAddr]
	mov al, 0
	call memset


	popa
	ret