; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                              VMM Reference Table
;                          12/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _VMM_RefInc
global _VMM_RefDec

TABLE_BASE	equ 0xFFFF800000000000

bits 64
default rel

; RAX = Physical Page Address
ALIGN 0x100
_VMM_RefInc:
	push rbp							; Save Modified Registers
	push rax
	push rbx
	mov rbx, rax						; Calculate Offset into Reference Table
	shr rbx, 9
	mov rax, 0x7FFFFFFFFFFFF8
	and rbx, rax
	mov rbp, TABLE_BASE					; Base Address of Reference Table
	call _Lock							; Lock the Reference Table
	inc QWORD [rbp + rbx]				; Increment Reference Count
	mov rax, [rbp + rbx]				; RAX = Reference Count
	call _Unlock						; Unlock the Reference Table
	pop rbx								; Restore Modified Registers
	pop rax
	pop rbp
	ret									; Return

; RAX = Physical Page Address
; Returns RAX = New Count
ALIGN 0x100
_VMM_RefDec:
	push rbp							; Save Modified Registers
	push rbx
	mov rbx, rax						; Calculate Offset into Reference Table
	shr rbx, 9
	mov rax, 0x7FFFFFFFFFFFF8
	and rbx, rax
	mov rbp, TABLE_BASE					; Base Address of Reference Table
	call _Lock							; Lock the Reference Table
	dec QWORD [rbp + rbx]				; Decrement Reference Count
	mov rax, [rbp + rbx]				; RAX = Reference Count
	call _Unlock						; Unlock the Reference Table
	pop rbx								; Restore Modified Registers
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
