; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                              VMM Reference Table
;                          10/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _VMM_RefInc
global _VMM_RefDec

TABLE_BASE	equ 0xFFFF800000000000

bits 64
default rel

; RAX = Physical Page Address
; Returns RAX = New Count
_VMM_RefInc:
	push rbp
	push rbx
	mov rbx, rax
	shr rbx, 11
	mov rax, 0xFFFFFFFFFFFFF8
	and rbx, rax
	mov rbp, TABLE_BASE
	call _Lock
	inc QWORD [rbp + rbx]
	mov rax, [rbp + rbx]
	call _Unlock
	pop rbx
	pop rbp
	ret

; RAX = Physical Page Address
; Returns RAX = New Count
_VMM_RefDec:
	push rbp
	push rbx
	mov rbx, rax
	shr rbx, 11
	mov rax, 0xFFFFFFFFFFFFF8
	and rbx, rax
	mov rbp, TABLE_BASE
	call _Lock
	dec QWORD [rbp + rbx]
	mov rax, [rbp + rbx]
	call _Unlock
	pop rbx
	pop rbp
	ret

ALIGN 0x100
_Lock:
	lock bts QWORD [VMM_LOCK], 0		; Attempt to acquire lock
	jnc .return							; If acquired return
	.LockWait:
		pause							; Tell CPU we are SpinLocking
		bt QWORD [VMM_LOCK], 0			; Check if we are still locked
		jc .LockWait					; If so, continue SpinLock
	jmp _Lock							; Retry acquiring lock
	.return:
		ret								; Return

ALIGN 0x100
_Unlock:
	mov QWORD [VMM_LOCK], 0				; Clear VMM_LOCK
	ret									; Return

section .data
ALIGN 0x100
VMM_LOCK	dq 0
