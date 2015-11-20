%include "./include/cpu/cpuid.inc"

[global _APIC_init]




SECTION .text ; Code area, Only Executable code please
align 0x00001000

[bits 32]

_APIC_init:
	call _PIC_init
	bt DWORD [CPUFLAGS.Standard], 9
	jnc .Error
	call Probe_RSDP
	jc .Error
	mov eax, RSDP
	xchg bx, bx
	ret
	.Error:
		cli
		hlt
		jmp .Error

Probe_RSDP:
	movzx edi, WORD [0x0000040E]
	shl edi, 4
	mov edx, 64
	.EBDA_Loop:
		mov ecx, 8
		push edi
		mov esi, RSDP_STRING
		rep cmpsb
		pop edi
		je .Found
		add edi, 0x10
		dec edx
		jnz .EBDA_Loop
	mov edx, 8192
	mov edi, 0x000E0000
	.BIOS_Loop:
		mov ecx, 8
		push edi
		mov esi, RSDP_STRING
		rep cmpsb
		pop edi
		je .Found
		add edi, 0x10
		dec edx
		jnz .BIOS_Loop
	stc
	ret
	.Found:
		mov ecx, 5
		cmp BYTE [edi + 0x0F], 0
		je .CopyRSDP
		add ecx, 4
		.CopyRSDP:
			mov esi, edi
			mov edi, RSDP
			rep movsd
		clc
		ret

_PIC_init:
	; Remap PIC
	mov dx, 0x20
	mov al, 0x11 ; INIT
	out dx, al
	mov dx, 0xA0
	out dx, al

	mov dx, 0x21 ; MAP
	mov al, 0x20
	out dx, al ; First PIC at int 0x20
	mov dx, 0xA1
	mov al, 0x28
	out dx, al ; Second PIC at int 0x28

	mov dx, 0x21 ; ICW3
	mov al, 0x04
	out dx, al ; First PIC uses 0x4 to talk to second
	mov dx, 0xA1
	mov al, 0x02 ; Second PIC uses 0x2 to talk to first
	out dx, al

	mov dx, 0x21 ; ICW4
	mov al, 0x01
	out dx, al
	mov dx, 0xA1
	out dx, al

	; Mask All INTs
	mov al, 0xFF
	out dx, al
	mov dx, 0x21
	out dx, al
	ret




SECTION .data
align 4

RSDP:
	.Signiture times 8 db 0
	.Checksum db 0
	.OEMID times 6 db 0
	.Revision db 0
	.RSTD_p dd 0
	.Length dd 0
	.XSTD_P dq 0
	.ExtChecksum db 0
	db 0, 0, 0




SECTION .rodata
align 4

RSDP_STRING db "RSD PTR "
