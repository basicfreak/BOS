; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                 PMM Allocator
;                          07/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This is far from being done...
; Needs to be separated into <1MB <16MB <4GB and >4GB for all the driver needs.
; Currently this just starts from the bottom and works upward in memory...

global _PMM_alloc
bits 64
default rel
align 0x800

%include 'lock.inc'

; RCX = Length
; Returns RAX = Base Address
_PMM_alloc:
	test rcx, 0xFFF
	jz .NoIssue
	xor rax, rax
	stc
	ret
  .NoIssue:
	push rsi
	push rcx
	push rdx
	push r8
	mov rsi, PMM_STACK
	mov r8, rcx
	call _Lock
	mov rcx, [PMM_Entries]
	test rcx, rcx
	jz .OOM
	.EntryLoop:
		lodsq
		xchg rdx, rax
		lodsq
		cmp rax, r8
		jge .FoundSpace
		loop .EntryLoop
	.OOM:
		mov rax, -1
		stc
		jmp .Return
	.FoundSpace:
		sub rax, r8
		sub [Free_RAM], r8
		add [Used_RAM], r8
		mov [rsi - 8], rax
		xchg rax, rdx
		add rax, rdx
		test rdx, rdx
		jz .RemoveEntry
		clc
	.Return:
		call _Unlock
		pop r8
		pop rdx
		pop rcx
		pop rsi
		ret
	.RemoveEntry:
		push rdi
		mov rdi, rsi
		shl rcx, 1
		sub rdi, 16
		rep movsq
		pop rdi
		dec QWORD [PMM_Entries]
		clc
		jmp .Return
