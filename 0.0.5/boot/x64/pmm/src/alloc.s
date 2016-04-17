; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                 PMM Allocator
;                          08/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This is tested to work, though not optimized

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
	test rcx, 0xFFF						; Check for bad inputs
	jz .NoIssue
	cmp rax, 4
	jb .NoIssue
	xor rax, rax
	stc
	ret
  .NoIssue:
	push rsi							; Save Modified Registers
	push rbx
	push rcx
	push rdx
	push r8
	push r9
	push r10
	mov r8, rcx							; Store Requested Length in r8
	test rax, rax
	jz .HighMemAlloc					; Attempt to allocate >4GB
	cmp rax, 1
	je .NormMemAlloc					; Attempt to allocate >16MB < 4GB
	cmp rax, 2
	je .LowMemAlloc						; Attempt to allocate >1MB < 16MB
  .RealModeAlloc:						; Attempt to allocate <1MB
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
	call _Lock							; Lock the PMM
  .ReEnt:
	mov rsi, PMM_STACK					; Source = PMM_STACK
	mov rcx, [PMM_Entries]				; Count = PMM_Entries
	test rcx, rcx						; If !Count
	jz .OOM								;    Out Of Memory
	.EntryLoop:
		lodsq							; Load Base
		xchg rdx, rax					;    into rdx
		lodsq							; Load Length into rax
		cmp rax, r8						; If Length < Requested Length
		jb .NotWithinRange				;    This entry will not work
		cmp rdx, r9						; If Max Range > Base
		ja .NotWithinRange				;    This entry is out of range

		mov rbx, rdx					; RBX = Base
		add rbx, rax					;    += Length

		cmp rdx, r10					; If Min Range >= Base
		jae .InRange					;    We are within range
		cmp rbx, r10					; If Min Range < (Base + Length)
		jb .NotWithinRange				;    This entry is out of range

	  .InRange:
		cmp rbx, r9						; if Max Range > (Base + Length)
		ja .FoundSpaceStart				;    Allocate from start of entry
		jbe .FoundSpaceEnd				; else    Allocate from end of entry
	  .NotWithinRange:
		loop .EntryLoop					; Loop until out of entries
	test r10, r10						; if Range = <1MB
	jz .OOM								;    Can't go lower, Out Of Memory
	bt r9, 63							; if Range = >4GB
	jc .GotoNormRange					;    Switch to range >16MB <4GB
	bt r9, 31							; if Range = >16MB <4GB
	jc .GotoLowRange					;    Switch to range >1MB <16MB
	mov r9, 0xFF000						; if Range = >1MB <16MB
	xor r10, r10						;    Switch to range <1MB
	jmp .ReEnt							; Reenter function
  .GotoNormRange:
	mov r9,  0xFFFFF000
	mov r10, 0x1000000
	jmp .ReEnt
  .GotoLowRange:
	mov r9, 0xFFF000
	mov r10, 0x100000
	jmp .ReEnt
	.OOM:
		mov rax, -1						; OOM Error Code = -1
		stc								; Set Error Flag
		jmp .ReturnNoUpdate				; Goto .Return
	.FoundSpaceStart:
		sub [rsi - 8], r8				; Subtract Requested Length from Length
		add [rsi - 16], r8				; Add Requested Length to Base
		xchg rax, rdx					; Move Base into rax
		mov rdx, [rsi - 8]				; Move new Length into rdx
		test rdx, rdx					; If New Length == 0
		jz .RemoveEntry					;    We need to remove this entry
		clc								; Clear Carry Flag (No Error)
		jmp .Return						; Goto .Return
	.FoundSpaceEnd:
		sub rax, r8						; Subtract Requested Length from Length
		mov [rsi - 8], rax				; Save Length
		xchg rax, rdx					; Swap Base and Length Registers
		add rax, rdx					; Add Length to Base
		test rdx, rdx					; If Length == 0
		jz .RemoveEntry					;    We need to remove this entry
		clc								; Clear Carry Flag (No Error)
	.Return:
		sub [Free_RAM], r8				; Adjust Free_RAM
		add [Used_RAM], r8				; Adjust Used_RAM
	.ReturnNoUpdate:
		call _Unlock					; Unlock the PMM
		pop r10							; Restore Modified Registers
		pop r9
		pop r8
		pop rdx
		pop rcx
		pop rbx
		pop rsi
		ret								; Return
	.RemoveEntry:
		push rdi						; Save Destination
		mov rdi, rsi					; Destination = Source
		shl rcx, 1						; Count *= 2
		sub rdi, 16						; Destination -= 16
		rep movsq						; Move entries to cover up this hole
		pop rdi							; Restore Destination
		dec QWORD [PMM_Entries]			; Decrement PMM_Entries
		clc								; Clear Carry Flag (No Error)
		jmp .Return						; Goto .Return
