; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0006
;                                  String Tree
;                          18/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 64
default rel

%include "VMM.inc"

StringTree	equ 0xFFFF808000000000		; That's 3.94 GB of Tree!
SymbolPTs	equ	0xFFFF8080FC000000		; Max of 8,388,096 Million Functions
SymStr		equ 0xFFFF8080FFFFF000		; Max Function Name = 1023 Chars

global _StringTree_init
global _FindSymbolID
global _AddSymbolID

section .init

_StringTree_init:
	push rax							; Save Modified Registers
	push rbx
	push rcx
	push rdx

	mov rdx, SymStr
	mov ax, 0xFF0
	mov bl, 1
	call _VMM_map
	mov rdx, SymbolPTs
	call _VMM_map
	call _AllocatePages
	inc QWORD [UsedPages]
	dec QWORD [FreePages]

	pop rdx								; Restore Modified Registers
	pop rcx
	pop rbx
	pop rax
	ret

section .text

_FindSymbolID:
	push rsi
	push rbp
	mov rbp, StringTree
	call _Lock
	call _ConvertName
	mov rsi, SymStr
	.CharLoop:
		lodsb
		test al, al
		jz .FoundID
		movzx eax, al
		shl rax, 3
		mov rax, [rbp + rax]
		test rax, rax
		jz .NotFound
		mov rbp, rax
		jmp .CharLoop
	.FoundID:
		mov rax, [rbp]
  .Return:
	call _Unlock
	pop rbp
	pop rsi
	ret
  .NotFound:
	stc
	jmp .Return

; ds:rsi = Function Name
; rdx = Function Pointer
_AddSymbolID:
	push rcx
	push rbx
	push rsi
	push rbp
	push rdx
	mov rbp, StringTree
	call _Lock
	call _ConvertName
	mov rsi, SymStr
	.CharLoop:
		lodsb
		test al, al
		jz .AddID
		movzx ebx, al
		shl rbx, 3
		mov rbx, [rbp + rbx]
		test rbx, rbx
		jnz .HaveChar

		mov rbx, [FreePages]
		test rbx, rbx
		jnz .WeHaveSpace
		call _AllocatePages
	  .WeHaveSpace:
		mov rcx, StringTree
		mov rbx, [UsedPages]
		shl rbx, 9
		add rbx, rcx
		movzx ecx, al
		shl rcx, 3
		mov [rbp + rcx], rbx
		inc QWORD [UsedPages]
		dec QWORD [FreePages]
	  .HaveChar:
		mov rbp, rbx
		jmp .CharLoop
	.AddID:
		mov rax, [Symbols]
		mov [rbp], rax
		mov rbp, SymbolPTs
		mov rbx, rax
		shl rbx, 3
		add rbp, rbx
		mov [rbp], rdx
		inc QWORD [Symbols]
		inc rax
		test rdx, 0x1FF
		jz .Return
		call _AllocatePTs
  .Return:
	call _Unlock
	pop rdx
	pop rbp
	pop rsi
	pop rbx
	pop rcx
	ret

_AllocatePTs:
	push rax
	push rdx
	push rbx
	mov rdx, SymbolPTs
	mov rbx, [Symbols]
	shl rbx, 3
	add rdx, rbx
	mov rcx, 0x1000
	mov ax, 0xFF0
	call _VMM_map
	xchg rdi, rdx
	xor rax, rax
	mov ecx, 512
	rep stosq
	xchg rdx, rdi
	pop rbx
	pop rdx
	pop rax
	ret

_AllocatePages:
	push rdx
	push rax
	push rdi
	push rcx

	mov rdx, StringTree
	mov rax, [UsedPages]
	add rax, [FreePages]
	shl rax, 9
	add rdx, rax
	mov ax, 0xFF0
	mov bl, 1
	call _VMM_map
	jc .error
	add BYTE [FreePages], 8
	mov rdi, rdx
	xor rax, rax
	mov ecx, 512
	rep stosq
  .error:

	pop rcx
	pop rdi
	pop rax
	pop rdx
	ret

; ds:rsi = name in
; es:SymStr = name out
_ConvertName:
	push rsi
	push rdi
	push rax
	mov rdi, SymStr
	.CharLoop:
		lodsb
		test al, al
		jz .StoreChar
		sub al, 0x2F
		cmp al, 0xB
		jb .StoreChar
		sub al, 7
		cmp al, 0x25
		jb .StoreChar
		sub al, 4
		cmp al, 0x25
		je .StoreChar
		dec al
	.StoreChar:
		stosb
		test al, al
		jnz .CharLoop
	clc
	pop rax
	pop rdi
	pop rsi
	ret

ALIGN 0x100
_Lock:
	lock bts QWORD [SYM_LOCK], 0		; Attempt to acquire lock
	jnc .return							; If acquired return
	.LockWait:
		pause							; Tell CPU we are SpinLocking
		bt QWORD [SYM_LOCK], 0			; Check if we are still locked
		jc .LockWait					; If so, continue SpinLock
	jmp _Lock							; Retry acquiring lock
	.return:
		ret								; Return

ALIGN 0x100
_Unlock:
	mov QWORD [SYM_LOCK], 0				; Clear VMM_LOCK
	ret									; Return

section .data

ALIGN 0x10
SYM_LOCK dq 0

Symbols dq 0
UsedPages dq 0
FreePages dq 0
