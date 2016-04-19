; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   VMM INIT
;                          12/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This works... Maybe... Not optimized yet, but it seems to be OK for now

global _init

bits 64
default rel

%include 'PMM.inc'
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

	shr rax, 21							; Calculate Pages needed
	mov rcx, rax
	shr rax, 9
	add rcx, rax
	shr rax, 9
	add rcx, rax
	add rcx, 13
	push rcx							; Push Total Pages Needed

	shl rcx, 12							; Pages to Bytes
	xor rax, rax						; High Memory Area (>4GB)
	call _PMM_alloc						; Allocate Memory
	jc .Error							; If CF=1 Error

	push rax							; Push Physical Base Address
	or ax, (_GLOBAL | _WRITE | _PRESENT); Or Address with 0x103

	mov rdi, (_PML4 + (8 * 0x100))		; Build Reference Table
	stosq								; PML4[256]
	mov rdi, (_PDP + (0x1000 * 0x100))
	invlpg [rdi]						; Invalidate PDP
push rdi
push rax
xor rax, rax
mov rcx, 0x1000
and rdi, -0x1000
rep stosb
pop rax
pop rdi
	add rax, 0x1000

	mov rcx, r15						; Calculate PDs to create
	shr rcx, 39
	inc rcx
	mov rdx, (_PD + (0x200000 * 0x100))
	.AllocatePDs:
		stosq							; PDP [xxx]
		invlpg [rdx]					; Invalidate PD
push rdi
push rax
push rcx
mov rcx, 0x1000
xor rax, rax
mov rdi, rdx
and rdi, -0x1000
rep stosb
pop rcx
pop rax
pop rdi
		add rdx, 0x1000
		add rax, 0x1000
		loop .AllocatePDs

	mov rcx, r15						; Calculate PTs to create
	shr rcx, 30
	inc rcx
	mov rdi, (_PD + (0x200000 * 0x100))
	mov rdx, (_PT + (0x40000000 * 0x100))
	.AllocatePTs:
		stosq							; PD [xxx]
		invlpg [rdx]					; Invalidate PT
push rdi
push rax
push rcx
mov rcx, 0x1000
xor rax, rax
mov rdi, rdx
and rdi, -0x1000
rep stosb
pop rcx
pop rax
pop rdi
		add rdx, 0x1000
		add rax, 0x1000
		loop .AllocatePTs

	mov rcx, r15						; Calculate Page Entries to create
	shr rcx, 21
	inc rcx
	mov rdi, (_PT + (0x40000000 * 0x100))
	.AllocateRefTable:
		stosq							; Store Page Entry
		add rax, 0x1000
		loop .AllocateRefTable

	.RebuildMap:
		mov rdi, (_PT + (0x40000000 * 511) + (0x200000 * 511) + (0x1000 * 511) + (8 * 503))
		mov rdx, 0xFFFFFFFFFFFF7000
		mov rcx, 9
		.TempMap:
			stosq						; Temporarily Map New PDIR
			invlpg [rdx]
			add rax, 0x1000
			add rdx, 0x1000
			loop .TempMap
		sub rdx, 0x8000

		mov rdi, rdx
		push rax
		xor rax, rax
		mov rcx, 0x8000
		rep stosb
		pop rax

		sub rax, 0x8000
		mov rdi, rdx					; Destination = -8000h
		pop rdx							; Get Physical Address of Reference Table
		push rdx
		or rdx, (_GLOBAL | _WRITE | _PRESENT)
		push rax						; Push PML4 Physical Address

		mov [rdi + 0xFF0], rax			; PML4[510] (Recursive)
		mov [rdi + 0x800], rdx			; PML4[256] (Reference Table)
		add rax, 0x1000
		mov [rdi], rax					; PML4[0] (Low Memory)
		add rax, 0x1000
		mov [rdi + 0xFF8], rax			; PML4[511] (High Memory)
		add rax, 0x1000
		add rdi, 0x1000
		stosq							; PML4[0] PDP [0]
		add rdi, 0x1FF0
		add rax, 0x1000
		stosq							; PML4[511] PDP [511]
		add rax, 0x1000
		stosq							; PML4[0] PDP [0] PD [0]
		add rdi, 0xFF8
		add rax, 0x1000
		stosq							; PML4[511] PDP [511] PD [0]
		add rax, 0x1000
		stosq							; PML4[511] PDP [511] PD[1]

		mov rdi, 0xFFFFFFFFFFFFD000		; Map Low Memory
		mov rcx, 0x40					; 0-40000
		mov rax, (_GLOBAL | _WRITE | _PRESENT)
		.MapLowMem:
			stosq
			add rax, 0x1000
			loop .MapLowMem
		mov rdi, 0xFFFFFFFFFFFFE800		; Map High Memory 100000-400000
		mov rcx, 0x300
		mov rax, 0x100103
		.MapHighMem:
			stosq
			add rax, 0x1000
			loop .MapHighMem

	pop rax								; Pop PML4 Physical Address
	and ax, 0xF000						; Remove Paging Flags
	mov cr3, rax						; Set CR3 to PML4

	xor rax, rax						; 0 the Reference Table
	mov rdi, 0xFFFF800000000000
	mov rcx, r15
	shr rcx, 12
	rep stosq

	pop r15								; Update Reference Table for itself
	pop rcx
	call .UpdateReferenceTable
	mov r15, 0							; Update Reference Table for Low Memory
	mov rcx, 0x10
	call .UpdateReferenceTable
	mov r15, 0x100000					; Update Reference Table for High Memory
	mov rcx, 0x300
	call .UpdateReferenceTable

	.Return:
		pop r15							; Restore Modified Registers
		pop rdx
		pop rax
		pop rcx
		pop rdi
		ret								; Return
	.Error:
		pop rcx							; Fix Stack
		stc								; CF = 1
		jmp .Return						; goto .Return

	.UpdateReferenceTable:
		mov rax, r15					; r15 = physical address
		call _VMM_RefInc				; Increase Reference Count
		add r15, 0x1000					; r15 += Page Size
		loop .UpdateReferenceTable		; Loop
		ret								; Return
