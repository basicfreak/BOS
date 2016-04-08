; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                 PMM Allocator
;                          08/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This is tested to work, though not optimized or commented yet ;)

global _PMM_alloc
bits 64
default rel
align 0x800

%include 'lock.inc'

; RCX = Length
; RAX = Type (0 = >4GB; 1 = <4GB; 2 = <16MB; 3 = <1MB)
; This will attempt to allocate as high in the range as possible (maybe)
;   It is allowed to allocate in a lower range if it's primary doesn't exist.
; Returns RAX = Base Address
_PMM_alloc:
	test rcx, 0xFFF
	jz .NoIssue
	cmp rax, 4
	jb .NoIssue
	xor rax, rax
	stc
	ret
  .NoIssue:
	push rsi
	push rbx
	push rcx
	push rdx
	push r8
	push r9
	push r10
	mov r8, rcx
	test rax, rax
	jz .HighMemAlloc
	cmp rax, 1
	je .NormMemAlloc
	cmp rax, 2
	je .LowMemAlloc
  .RealModeAlloc:
	mov r9, 0xFF000
	xor r10, r10
	jmp .Alloc
  .LowMemAlloc:
	mov r9,  0xFFF000
	mov r10, 0x100000
	jmp .Alloc
  .NormMemAlloc:
	mov r9,  0xFFFFF000
	mov r10, 0x1000000
	jmp .Alloc
  .HighMemAlloc:
	mov r9,  0xFFFFFFFFFFFFF000
	mov r10, 0x100000000
  .Alloc:
	call _Lock
  .ReEnt:
	mov rsi, PMM_STACK
	mov rcx, [PMM_Entries]
	test rcx, rcx
	jz .OOM
	.EntryLoop:
		lodsq
		xchg rdx, rax
		lodsq
		cmp rax, r8
		jb .NotWithinRange
		cmp rdx, r9
		ja .NotWithinRange

		mov rbx, rdx
		add rbx, rax

		cmp rdx, r10
		jae .InRange
		cmp rbx, r10
		jb .NotWithinRange

	  .InRange:
		cmp rbx, r9
		ja .FoundSpaceStart
		jbe .FoundSpaceEnd
	  .NotWithinRange:
		loop .EntryLoop
	test r10, r10
	jz .OOM
	bt r9, 63
	jc .GotoNormRange
	bt r9, 31
	jc .GotoLowRange
	mov r9, 0xFF000
	xor r10, r10
	jmp .ReEnt
  .GotoNormRange:
	mov r9,  0xFFFFF000
	mov r10, 0x1000000
	jmp .ReEnt
  .GotoLowRange:
	mov r9, 0xFFF000
	mov r10, 0x100000
	jmp .ReEnt
	.OOM:
		mov rax, -1
		stc
		jmp .Return
	.FoundSpaceStart:
		sub [rsi - 8], r8
		add [rsi - 16], r8
		xchg rax, rdx
		mov rdx, [rsi - 8]
		test rdx, rdx
		jz .RemoveEntry
		clc
		jmp .Return
	.FoundSpaceEnd:
		sub rax, r8
		mov [rsi - 8], rax
		xchg rax, rdx
		add rax, rdx
		test rdx, rdx
		jz .RemoveEntry
		clc
	.Return:
		sub [Free_RAM], r8
		add [Used_RAM], r8
		call _Unlock
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		pop rbx
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
