; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   VMM UNMAP
;                          12/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _VMM_unmap

bits 64
default rel

%include 'RefTable.inc'
%include 'PMM.inc'
%include 'flags.inc'

align 0x800
; RAX = Virtual Address
; RAX should return 0
_VMM_unmap:
	push rsi							; Save Modified Registers
	push r10
	push r15

	mov r15, rax						; Save Virtual Address
	shr rax, 9							; Calculate Page Entry Offset
	mov r10, 0x7FFFFFFFF8
	and rax, r10
	mov rsi, _PT
	add rsi, rax
	lodsq								; Load current entry value
	and ax, 0xF000						; clear flags
	mov r10, rax						; Save Physical Address
	call _VMM_RefDec					; Decrement Reference Count
	test rax, rax						; If new count != 0
	jnz .StillUsed						; Don't free it.
	push rcx							; Save RCX
	mov rax, r10						; Physical Address
	call _PMM_free						; Free Physical Address
	pop rcx								; Restore RCX
	.StillUsed:
		mov QWORD [rsi - 8], 0			; 0 The Page Entry
		invlpg [r15]					; Invalidate the Virtual Address

	pop r15								; Restore Modified Registers
	pop r10
	pop rsi
	ret									; Return
