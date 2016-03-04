; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                                  Memory  Map
;                          04/03/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This should be interesting...

section .text
bits 16

MemoryStack equ 0xE000

global INIT_MMAP

INIT_MMAP:
	pushad								; Save registers
	mov di, MemoryStack					; 0 Out Memory Stack Page 1
	mov bp, di							; BP will be our pointer into Mem Stack
	mov cx, 0x1000						; Page Size = 4KB
	xor al, al							; Data = 0
	rep stosb							; Clear Mem Stack
	mov eax, 0xE820						; Function = 0xE820 (Mem Map)
	mov edx, 'PAMS'						; EDX = "SMAP"
	xor ebx, ebx						; Base = 0
	mov ecx, 24							; Size = 24
	mov di, MemoryEntry					; Destination = ES:MemoryEntry
	mov si, di
	mov DWORD [di + 20], 1 				; FORCE ACPI 3.x
	int 0x15							; Int 15/E820
	jc .error							; If failed, Error
	mov edx, 'PAMS'						; EDX = "SMAP"
	cmp eax, edx						; If EAX != "SMAP"
	jne .error							;   Error

; Do I really Need this, If not this can be optimized so much more...
	test ebx, ebx						; If this the last entry
	jz .error							;   Error

	jmp .TestEntry						; Test the entry
	.EntryLoop:
		mov eax, 0xE820					; Function = 0xE820 (Mem Map)
		mov edx, 'PAMS'					; EDX = "SMAP"
		mov DWORD [di + 20], 1			; Force ACPI 3.x
		mov ecx, 24						; Size = 24
		int 0x15						; Int 15/E820
		jc .error						; If failed, Error
		mov edx, 'PAMS'					; EDX = "SMAP"
		cmp eax, edx					; If EAX != "SMAP"
		jne .error						;   Error
	.TestEntry:
		jcxz .ContinueLoop				; If returned size = 0, ContinueLoop
		cmp cl, 20						; If returned size <= 20
		jbe .NotACPI3					;   Not ACPI3.x
		test BYTE [di + 20], 1			; If this is not a valid entry
		jz .ContinueLoop				;   ContinueLoop
	.ProcessEntry:
		mov eax, [si + 8]
		mov edx, [si + 12]
		cmp DWORD [si + 0x10], 1		; If not a "Free Memory" type
		jne .ContinueLoop				;  ContinueLoop
		mov eax, [si]					; EDX:EAX = 64-Bit Base Address
		mov edx, [si + 4]
		xor ecx, ecx					; ECX - stores change in Base Address
		test eax, 0xFFF					; If this page aligned
		jz .BaseAligned					;   Skip alignment
		mov cx, 0x1000					; Align base to Page Boundary
		push ax
		and ax, 0xFFF
		sub cx, ax						; Offset = PageSize - (Base & 0xFFF)
		pop ax
		add edx, ecx
		.BaseAligned:
			mov [bp], eax				; Store Aligned Base Address in Stack
			mov [bp + 4], edx
			mov eax, [si + 8]			; EDX:EAX = 64-Bit Length
			mov edx, [si + 12]
			sub eax, ecx				; Fix Length from Base Alignment
			and eax, 0xFFFFF000			; Align Length to Page Boundary
		mov [bp + 8], eax				; Store Aligned Length in Stack
		mov [bp + 12], edx
		add bp, 16						; Each entry is 16 bytes (2 QWORDS)
	.ContinueLoop:
		test ebx, ebx					; If there are more entries
		jnz .EntryLoop					;   EntryLoop
	.Done:
		clc								; Clear Carry Flag
	.Return:
		popad							; Restore registers
		ret								; Return to Caller
	.NotACPI3:
		mov ecx, [di + 8]				; ECX = Low (32) Length
		or ecx, [di + 12]				; ECX |= High (32) Length
		jz .ContinueLoop				; If ECX == 0, ContinueLoop
		jmp .ProcessEntry
	.error:
		stc								; Set Carry Flag
		jmp .Return						; Do Return instructions

section .data

align 0x20
MemoryEntry times 24 db 0
