; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   PMM Lock
;                          06/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _Lock
global _Unlock
global Total_RAM
global Used_RAM
global Free_RAM
global PMM_Entries

bits 64
default rel

ALIGN 0x100
_Lock:
	lock bts QWORD [PMM_LOCK], 0		; Attempt to acquire lock
	jnc .return							; If acquired return
	.LockWait:
		pause							; Tell CPU we are SpinLocking
		bt QWORD [PMM_LOCK], 0			; Check if we are still locked
		jc .LockWait					; If so, continue SpinLock
	jmp _Lock							; Retry acquiring lock
	.return:
		ret								; Return

ALIGN 0x100
_Unlock:
	mov QWORD [PMM_LOCK], 0				; Clear PMM_LOCK
	ret									; Return

section .data
ALIGN 0x100
PMM_LOCK	dq 0
Total_RAM	dq 0
Used_RAM	dq 0
Free_RAM	dq 0
PMM_Entries dq 0
