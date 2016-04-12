; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                              PMM Initialization
;                          07/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _init
section .init
bits 64
default rel

%include 'free.inc'
%include 'lock.inc'

_init:
	push rsi							; Save Modified Registers
	push rdi
	push rdx

	mov rsi, 0xE000						; MMAP was saved to 0xE000 by SYSINIT
	mov rdi, PMM_STACK					; PMM Stack Location
	mov rcx, 0x200						; Max MMAP Entries

	.EntryLoop:
		lodsq							; Load Base Address
		xchg rdx, rax					; into RDX
		lodsq							; Load Length into RAX
		mov rbx, rdx
		add rbx, rax					; RBX = Base + Length
		test rbx, rbx					; If RBX == 0
		jz .Done						;    We have read all existing entries
		cmp [Total_RAM], rbx			; Is RBX < Total_RAM
		ja .TRNG						;    Yes, Skip next step
		mov [Total_RAM], rbx			;    No, Total_RAM = RBX
	  .TRNG:
		cmp rdx, 0x30000				; Is Base < 12KB?
		jb .LowMemFix					;    Fix Low Memory
		cmp rdx, 0x301000				; Is Base > 3MB + 4KB?
		jae .ContinueFree				;    Continue
		cmp rdx, 0x100000				; Is Base > 1MB?
		jae .HighMemFix					;    Fix High Memory
		.FreeEntry:
			cmp rbx, 0x100000			; Is End Of Range < 1MB?
			jbe .ContinueFree			;    Free entry
			mov rax, 0x100000			; Length = 1MB
			sub rax, rdx				;        -= Base
		  .ContinueFree:
			call _PMM_free				; Free the entry
		loop .EntryLoop					; Loop until done.
	.Done:
		mov rax, [Total_RAM]			; Fix Used_RAM
		sub rax, [Free_RAM]				;    (Total_RAM - Free_RAM)
		mov [Used_RAM], rax
	pop rdx								; Restore Modified Registers
	pop rdi
	pop rsi
	mov rax, [Total_RAM]
	mov rbx, [Used_RAM]
	mov rcx, [Free_RAM]
	ret									; Return
	.LowMemFix:
		push rbx						; Save RBX
		mov rbx, 0x30000				; Adjust Low Memory to outside
		sub rbx, rdx					;    of the 0-12KB range
		add rdx, rbx
		sub rax, rbx
		pop rbx							; Restore RBX
		jmp .FreeEntry					; Continue Entry check
	.HighMemFix:
		mov rbx, 0x301000				; Adjust High Memory to outside
		sub rbx, rdx					;    of the 1MB-(3MB+4KB) range
		add rdx, rbx
		sub rax, rbx
		jmp .ContinueFree				; Free The Entry
