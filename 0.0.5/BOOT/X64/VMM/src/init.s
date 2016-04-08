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

section .init
global _init

_init:
	xchg bx, bx
	xor rax, rax
	mov rcx, 0x1000
	call _PMM_alloc
	xchg bx, bx
	mov rax, 1
	call _PMM_alloc
	xchg bx, bx
	mov rax, 2
	call _PMM_alloc
	xchg bx, bx
	mov rax, 3
	call _PMM_alloc
	xchg bx, bx
	jmp _init
	stc
	ret
