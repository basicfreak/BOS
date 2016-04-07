; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   PMM Freer
;                          06/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _PMM_free
bits 64
default rel
align 0x800

%include 'lock.inc'

; RAX = Length; RDX = Base
_PMM_free:
	test rax, 0xFFF
	jnz .InputError
	test rdx, 0xFFF
	jnz .InputError

	push rsi
	push rcx
	push rbx
	push rdi
	push rax
	push rdx
	push r8
	push r9
	push r10

	mov r8, rdx ;base
	mov r9, rax ;length
	add rdx, rax ;base+length
	mov r10, rdx
	mov rsi, PMM_STACK
	call _Lock
	mov rcx, PMM_Entries
	test rcx, rcx
	jz .AddEntry
	.EntryLoop:
		lodsq
		xchg rdx, rax
		lodsq
		mov rbx, rdx
		add rbx, rax
		cmp rbx, r8
		je .AddAbove
		cmp rdx, r10
		je .AddBelow
		loop .EntryLoop
	.AddEntry:
		inc QWORD [PMM_Entries]
		mov [esi], r8
		mov [esi + 8], r9
		jmp .FixUsage
	.AddBelow:
		sub [esi - 16], r9
		jmp .FixUsage
	.AddAbove:
		add [esi - 8], r8
	.FixUsage:
		add [Free_RAM], r10
		sub [Used_RAM], r10
	call _Unlock
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rax
	pop rdi
	pop rbx
	pop rcx
	pop rsi
	ret
	.InputError:
		stc
		ret
