; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   PMM Freer
;                          07/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _PMM_free
bits 64
default rel
align 0x800

%include 'lock.inc'

; RAX = Length; RDX = Base
_PMM_free:
	test rax, 0xFFF						; Check for bad inputs
	jnz .InputError
	test rdx, 0xFFF
	jnz .InputError

	push rsi							; Save Modified Registers
	push rcx
	push rbx
	push rdi
	push rax
	push rdx
	push r8
	push r9
	push r10

	mov r8, rdx							; Store Base_to_free in r8
	mov r9, rax							; Store Length_to_free in r9
	add rdx, rax						; Base += Length_to_free
	mov r10, rdx						; Store Base+Length_to_free in r10
	mov rsi, PMM_STACK					; PMM Stack Location
	call _Lock							; Lock the PMM
	mov rcx, [PMM_Entries]				; Get current number of entries
	test rcx, rcx						; If No entries
	jz .AddEntry						;    Just Add a new one
	.EntryLoop:
		lodsq							; Load Base Address
		xchg rdx, rax					; into RDX
		lodsq							; Load Length into RAX
		mov rbx, rdx					; RBX = Base
		add rbx, rax					;    += Length
		cmp rbx, r8						; Is Base+Length = Base_to_free?
		je .AddAbove					;    yes, add above entry
		cmp rdx, r10					; Is Base = End_to_free?
		je .AddBelow					;    yes, add below entry
		loop .EntryLoop					; Loop through all entries
	.AddEntry:
		inc QWORD [PMM_Entries]			; Increase PMM_Entries count
		mov [rsi], r8					; Add Base to PMM_Stack
		mov [rsi + 8], r9				; Add Length to PMM Stack
		jmp .FixUsage					; We still need to fix Free/Used_RAM
	.AddBelow:
		sub [rsi - 16], r9				; Subtract Length_to_free from Base
	.AddAbove:
		add [rsi - 8], r8				; Add Length_to_free to Length
	.FixUsage:
		add [Free_RAM], r10				; Add End_to_free to Free_RAM
		sub [Used_RAM], r10				; Subtract End_to_free from Used_RAM
	call _Unlock						; Unlock the PMM
	pop r10								; Restore Modified Registers
	pop r9
	pop r8
	pop rdx
	pop rax
	pop rdi
	pop rbx
	pop rcx
	pop rsi
	ret									; Return
	.InputError:
		stc								; Set CF
		ret								; Return
