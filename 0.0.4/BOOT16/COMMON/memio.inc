;----------------------------------------------------
;                   memio.inc
;                   2015-07-16
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __MEMIO_INC__INCLUDED__
%define __MEMIO_INC__INCLUDED__

[bits 16]

;----------------------------------------------------
;                       memset
;   INPUT: EDI - destination ECX - count AL - value
;----------------------------------------------------
memset:
	cli
	push ds
	push es
	db 0x66, 0x67
		mov  ebx, cr0			; switch to pmode by
	or bl,1					; set pmode bit
	db 0x66, 0x67
		mov  cr0, ebx

	db 0x66, 0x67
		push ebx
	mov bx, 10h				; set pmode Data GDT ent
	mov ds, bx
	mov es, bx

	jmp 8:(.pmode + 0xBE00)			; set pmode Code GDT ent
[bits 32]
	.pmode:
		cld
		.loop:
			db 0x66, 0x67
			mov BYTE [ES:EDI], al			; copy image to its protected mode address
			db 0x66, 0x67
			inc EDI
			db 0x66, 0x67
			dec ecx
			jnz .loop
 
[bits 16]
	db 0x66, 0x67
	pop ebx
	and bl,0xFE 			; back to realmode
	db 0x66, 0x67
	mov  cr0, ebx			; by toggling bit again
	jmp 0x0:(.done + 0xBE00)
	.done:
		pop es				; load real mode
		pop ds
		sti
	ret


;----------------------------------------------------
;                       memcpy
;  INPUT: ESI - source EDI - destination ECX - count
;   NOTE: This counts in DWORDS (4 Bytes per count)
;----------------------------------------------------
memcpy:
	cli
	push ds					; save real mode
	push es

	db 0x66, 0x67
	mov  ebx, cr0			; switch to pmode by
	or bl,1					; set pmode bit
	db 0x66, 0x67
	mov  cr0, ebx

	db 0x66, 0x67
	push ebx
	mov bx, 10h				; set pmode Data GDT ent
	mov ds, bx
	mov es, bx

	jmp 8:(.pmode + 0xBE00)			; set pmode Code GDT ent
[bits 32]
	.pmode:
		cld
   		db 0x66, 0x67
		rep	movsd			; copy image to its protected mode address
 
[bits 16]
	db 0x66, 0x67
	pop ebx
	and bl,0xFE 			; back to realmode
	db 0x66, 0x67
	mov  cr0, ebx			; by toggling bit again
	jmp 0x0:(.done + 0xBE00)
	.done:
		pop es				; load real mode
		pop ds
		sti
	ret

;----------------------------------------------------
;                       memmap
;                    INPUT: NONE
;          OUTPUT: 0xE000 - 0xE3FF Mem Map,
;              WORD mmap_ents = entries
;----------------------------------------------------
memmap:
	pusha
	mov di, 0x2200 ;(ES:DI = 0xE000)
	xor ebx, ebx
	xor bp, bp
	mov edx, DWORD 'PAMS'
	mov eax, DWORD 0xE820
	; REM ES = 0x0BE0, DS = 0x0000
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
		mov [mmap_ents], bp
		mov di, 0x2E00 ; [es:di] = 0xEC00
		mov eax, DWORD 0xE000
		mov DWORD [es:di + 1], eax
		mov WORD [es:di + 5], bp
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
		mov edx, DWORD 0
		mov dx, ax
		db 0x66, 0x67
			shl edx, 10
		mov DWORD [mem_high], edx
		mov edx, DWORD 0
		mov dx, bx
		db 0x66, 0x67
			shl edx, 16
		add DWORD [mem_high], edx

	mov eax, DWORD 0
	int 0x12
	jc ERROR
	test ax, ax
	jz ERROR
	db 0x66, 0x67
		shl eax, 10
	mov DWORD [mem_low], eax

	mov di, 0x2E00
	mov DWORD [es:di + 32], eax
	mov eax, DWORD [mem_high]
	mov DWORD [es:di + 36], eax

	popa
	ret

mmap_ents dw 0
mem_high dd 0
mem_low dd 0

%endif
