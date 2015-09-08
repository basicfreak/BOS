;----------------------------------------------------
;                  STAGETWO.ASM
;                 (FAT12 FLOPPY)
;          Bootsector Version 0.0.2.1
;                   2015-07-04
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]
[org 0xBE00]
mov ax, 0x0BE0
mov es, ax
xor ax, ax
mov ds, ax
mov fs, ax
mov gs, ax
;----------------------------------------------------
;        Get Important BPB info from stack
;----------------------------------------------------
A20:
	.A20BIOS:
		mov ax, 0x2401
		int 0x15
		jnc .Continue
	.A20FAST:
		in al, 0x92
		test al, 2 				; Check if A20 is already on
		jnz .Continue 			; if is jump past
		or al, 2
		and al, 0xFE
		out 0x92, al 			; Set A20 Line
	.Continue:

		xchg bx, bx
		push ds
		push es
		mov  eax, cr0          ; switch to pmode by
		or al,1                ; set pmode bit
		mov  cr0, eax
		lgdt [gdtinfo]         ; load gdt register
		jmp 8:pmode
[bits 32]
	pmode:
		mov bx,10h
		mov ds,bx
		mov es, bx
		push eax

		jmp 0x28:urmode
[bits 16]
	urmode:
		mov eax, cr3
		mov DWORD [tempcr3], eax
		mov eax, cr0
		and eax, 0x7FFFFFFE
		mov cr0, eax
		jmp 0:rmode
	rmode:
		xor ax, ax
		mov ds, ax
		mov es, ax
		mov ax, 0x4F02
		mov bx, 0x0113
		int 0x10
		mov eax, DWORD [tempcr3]
		mov cr3, eax
		mov  eax, cr0          ; switch to pmode by
		or eax,0x00000001                ; set pmode bit
		mov  cr0, eax
		jmp 8:pmodetwo
[bits 32]
	pmodetwo:
		mov ax, 0x10
		mov ds, ax
		mov es, ax
		hlt

;----------------------------------------------------
;                        GDT
;----------------------------------------------------
	gdtinfo:
	   dw gdt_end - gdt - 1   ;last byte in table
	   dd gdt                 ;start of table

	gdt:
	nullgdt         dd 0,0        ; entry 0 is always unused
	pmr0code	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0
	pmr0data	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0
	pmr3code	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11111111b, 0
	pmr3data	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11111111b, 0
	rmr0code	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 10001111b, 0
	gdt_end:
tempcr3 dd 0