; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   VMM INIT
;                          07/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This is not real, only used to test the PMM - next update it *will* be real!

global _init

bits 64
default rel

%include 'PMM.inc'
%include 'map.inc'
%include 'unmap.inc'
%include 'flags.inc'
%include 'RefTable.inc'

section .init
global _init

_init:
	push rdi							; Save Modified Registers
	push rcx
	push rax
	push rdx
	push r15
	mov r15, rax						; Store Total_RAM in r15
; xchg bx, bx
	shr rax, 21
	mov rcx, rax
	shr rax, 9
	add rcx, rax
	shr rax, 9
	add rcx, rax
	add rcx, 13 ; I Think...
	push rcx
	shl rcx, 12
	mov rax, 1
	call _PMM_alloc
	jc .Error
	push rax
	or ax, (_GLOBAL | _WRITE | _PRESENT)
	mov rdi, (_PML4 + (8 * 0x100))
	stosq
	mov rdi, (_PDP + (0x1000 * 0x100))
	invlpg [rdi]
	add rax, 0x1000

	mov rcx, r15
	shr rcx, 39
	inc rcx
	mov rdx, (_PD + (0x200000 * 0x100))
	.AllocatePDs:
		stosq
		invlpg [rdx]
		add rdx, 0x200000
		add rax, 0x1000
		loop .AllocatePDs
	mov rcx, r15
	shr rcx, 30
	inc rcx
	mov rdi, (_PD + (0x200000 * 0x100))
	mov rdx, (_PT + (0x40000000 * 0x100))
	.AllocatePTs:
		stosq
		invlpg [rdx]
		add rdx, 0x40000000
		add rax, 0x1000
		loop .AllocatePTs
	mov rcx, r15
	shr rcx, 21
	inc rcx
	; mov rdx, 0xFFFF800000000000
	mov rdi, (_PT + (0x40000000 * 0x100))
	.AllocateRefTable:
		stosq
		; invlpg [rdx]
		; add rdx, 0x1000
		add rax, 0x1000
		loop .AllocateRefTable
xchg bx, bx
	.RebuildMap:

; [xxx] [xxx] [xxx] [xxx]

		mov rdi, (_PT + (0x40000000 * 511) + (0x200000 * 511) + (0x1000 * 511) + (8 * 503))
		mov rdx, 0xFFFFFFFFFFFF7000
		mov rcx, 9
		.TempMap:
			stosq
			invlpg [rdx]
			add rax, 0x1000
			add rdx, 0x1000
			loop .TempMap
		sub rdx, 0x8000
		sub rax, 0x8000
		mov rdi, rdx
		pop r15
		push r15
		or r15, (_GLOBAL | _WRITE | _PRESENT)
		push rax

		mov [rdi + 0xFF0], rax	; PML4[510] (Recursive)
		mov [rdi + 0x800], r15
		add rax, 0x1000
		mov [rdi], rax	; PML4[0] (Low Memory)
		add rax, 0x1000
		mov [rdi + 0xFF8], rax	; PML4[511] (High Memory)
		add rax, 0x1000
		add rdi, 0x1000
		stosq	; PML4[0] PDP [0]
		add rdi, 0x1FF0
		add rax, 0x1000
		stosq	; PML4[511] PDP [511]
		add rax, 0x1000
		stosq	; PML4[0] PDP [0] PD [0]
		add rdi, 0xFF8
		add rax, 0x1000
		stosq	; PML4[511] PDP [511] PD [0]
		; add rdi, 0xFF8
		add rax, 0x1000
		stosq	; PML4[511] PDP [511] PD[1]
xchg bx, bx
		mov rdi, 0xFFFFFFFFFFFFD000
		mov rcx, 0x40
		mov rax, (_GLOBAL | _WRITE | _PRESENT)
		.MapLowMem:
			stosq
			add rax, 0x1000
			loop .MapLowMem
		mov rdi, 0xFFFFFFFFFFFFE800	; 1MB into table
		mov rcx, 0x400
		mov rax, 0x100103
		.MapHighMem:
			stosq
			add rax, 0x1000
			loop .MapHighMem
xchg bx, bx
		pop rcx

		and cx, 0xF000
		mov cr3, rcx
		nop





xchg bx, bx


	pop r15
	pop rcx
	call .UpdateReferenceTable
	mov r15, 0
	mov rcx, 0x40
	call .UpdateReferenceTable
	mov r15, 0x100000
	mov rcx, 0x400
	call .UpdateReferenceTable



	xchg bx, bx
	.Return:
		pop r15
		pop rdx
		pop rax
		pop rcx
		pop rdi
		ret
	.Error:
		pop rcx
		stc
		jmp .Return

	.UpdateReferenceTable:
		mov rax, r15
		call _VMM_RefInc
		add r15, 0x1000
		loop .UpdateReferenceTable
		ret
