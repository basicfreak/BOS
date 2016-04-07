; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                              PMM Initialization
;                          06/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _init
section .init
bits 64
default rel

%include 'free.inc'
%include 'lock.inc'

_init:
	; call initialize_PMM
	push rsi
	push rcx
	push rbx
	push rdi
	push rax
	push rdx

	mov rsi, 0xE000
	mov rdi, PMM_STACK
	mov rcx, 0x400

	.EntryLoop:
		lodsq
		xchg rdx, rax
		lodsq
		mov rbx, rdx
		add rbx, rax
		test rbx, rbx
		jz .Done
		cmp [Total_RAM], rbx
		jb .TRNG
		mov [Total_RAM], rbx
	  .TRNG:
		; RDX = Base; RAX = Length; RBX = End
		cmp rdx, 0x30000
		jb .LowMemFix
		cmp rdx, 0x301000
		jae .ContinueFree
		cmp rdx, 0x100000
		jae .HighMemFix
		.FreeEntry:
			cmp rbx, 0x100000
			jbe .ContinueFree
			mov rax, 0x100000
			sub rax, rdx
		  .ContinueFree:
			call _PMM_free
		sub rcx, 2
		jmp .EntryLoop
	.Done:
		xor rax, rax
		rep stosq
		mov rax, [Total_RAM]
		sub rax, [Used_RAM]
		mov [Free_RAM], rax
	pop rdx
	pop rax
	pop rdi
	pop rbx
	pop rcx
	pop rsi
	ret
	.LowMemFix:
		mov rbx, 0x30000
		sub rbx, rdx
		add rdx, rbx
		sub rax, rbx
		jmp .FreeEntry
	.HighMemFix:
		mov rbx, 0x301000
		sub rbx, rax
		add rdx, rbx
		sub rax, rbx
		jmp .ContinueFree
