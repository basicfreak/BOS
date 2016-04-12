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
	mov r15, rax
	push 0
	; Create a PDP For Reference Table
	xor rax, rax
	mov rcx, 0x1000
	call _PMM_alloc
	jc .Error
	push rax
	mov rdi, (_PML4 + (8 * 0x100))
	or ax, (_GLOBAL | _WRITE | _PRESENT)
	stosq
	mov rdi, (_PDP  + (0x1000 * 0x100))
	invlpg [rdi]



	mov rcx, r15
	shr rcx, 39
	inc rcx
	mov rdx, (_PD + (0x200000 * 0x100))
	.AllocatePageDirs:
		push rcx
		xor rax, rax
		mov rcx, 0x1000
		call _PMM_alloc
		pop rcx
		jc .Error
		push rax
		or ax, (_GLOBAL | _WRITE | _PRESENT)
		stosq
		invlpg [rdx]
		add rdx, 0x40000000
		loop .AllocatePageDirs
	mov rdi, (_PD + (0x200000 * 0x100))
xchg bx, bx


; WHEN YOU GET BACK YOUR ISSUE IS BOTH ABOVE AND BELOW HERE!!!!

	mov rcx, r15
	shr rcx, 30
	inc rcx
	mov rdx, (_PT + (0x40000000 * 0x100))
	.AllocatePageTables:
		push rcx
		xor rax, rax
		mov rcx, 0x1000
		call _PMM_alloc
		pop rcx
		jc .Error
		push rax
		or ax, (_GLOBAL | _WRITE | _PRESENT)
		stosq
		invlpg [rdx]
		add rdx, 0x200000
		loop .AllocatePageTables
	mov rdi, (_PT + (0x40000000 * 0x100))

	mov rcx, r15
	; 1 : 512 Bytes Needed
	shr rcx, 9
	test rcx, 0xFFF
	jz .PageSized
	add rcx, 0x1000
	and rcx, 0xFFFFFFFFFFFFF000
  .PageSized:
	xor rax, rax
	call _PMM_alloc
	jc .Error
	push rax
	shr rcx, 12
	push rcx

xchg bx, bx
	mov r15, 0xFFFF800000000000
	or ax, (_GLOBAL | _WRITE | _PRESENT)
	.MapEntries:
		stosq
		invlpg [r15]
		add r15, 0x1000
		add rax, 0x1000
		loop .MapEntries

xchg bx, bx
	pop rcx
	pop r15
	.RefEntries:
		mov rax, r15
		call _VMM_RefInc
		add r15, 0x1000
		loop .RefEntries


xchg bx, bx
	.UpdateRefTable:
		pop rax
		test rax, rax
		jz .MoveMap
		call _VMM_RefInc
		jmp .UpdateRefTable

	.MoveMap:



	xchg bx, bx
	.Return:
		pop r15
		pop rax
		pop rcx
		pop rdi
		ret
	.Error:
		stc
		jmp .Return
