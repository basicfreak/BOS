; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                                  Memory  Map
;                          02/03/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This should be interesting...

section .text
bits 16

global INIT_MMAP

INIT_MMAP:
	pushad								; Save registers
	push es								; Save Extra Segment
	mov eax, 0x3000
	mov es, ax							; Extra Segment = 0x3000
	mov ax, 0xE820						; Function = 0xE820 (Mem Map)
	mov edx, 'PAMS'						; EDX = "SMAP"
	xor ebx, ebx						; Base = 0
	mov ecx, 24							; Size = 24
	mov di, 0							; Destination = ES:0
	mov DWORD [es:di + 20], 1 			; FORCE ACPI 3.x
	int 0x15							; Int 15/E820
	jc .error							; If failed, Error
	mov edx, 'PAMS'						; EDX = "SMAP"
	cmp eax, edx						; If EAX != "SMAP"
	jne .error							;   Error
	test ebx, ebx						; If this the last entry
	jz .error							;   Error
	jmp .TestEntry						; Test the entry
	.EntryLoop:
		mov eax, 0xE820					; Function = 0xE820 (Mem Map)
		mov DWORD [es:di + 20], 1		; Force ACPI 3.x
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
		test BYTE [es:di + 20], 1		; If this a skip entry
		jz .ContinueLoop				;   ContinueLoop
	.NotACPI3:
		mov ecx, [es:di + 8]			; ECX = Low (32) Length
		or ecx, [es:di + 12]			; ECX |= High (32) Length
		jz .ContinueLoop				; If ECX == 0, ContinueLoop
		add di, 24						; Increment Destination by size (24)
	.ContinueLoop:
		test ebx, ebx					; If there are more entries
		jnz .EntryLoop					;   EntryLoop
	.Done:
		mov al, 0xFF					; Data = 0xFF
		mov cx, 24						; Length = 24 (BYTES)
		rep stosb						; Store End of List Entry
		clc								; Clear Carry Flag
	.Return:
		pop es							; Restore Extra Statement
		popad							; Restore registers
		ret								; Return to Caller
	.error:
		stc								; Set Carry Flag
		jmp .Return						; Do Return instructions
