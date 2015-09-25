[bits 32]

[global API_Handler]
[extern PageDirectoryBase]
[extern _VMM_map_UNSAFE]
[extern _PMM_alloc]

API_LIST_BASE equ 0xE4110000
API_CODE_BASE equ 0xE4200000

API_CODE_TLB equ 0xFFF90800

API_Handler:
	mov eax, DWORD [esp + 52]					; Grab Function Number (stored in eax of the calling thread)
xchg bx, bx

	cmp al, 0x01
	je .FindAPI
	cmp al, 0x02
	je .AddAPI
	cmp al, 0xF0
	je .InatallAPI

	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread.

	.FindAPI:
xchg bx, bx
		; esi = function name pointer
		; RETURN eax = function location
		mov ebp, esi							; Save Pointer to Name
		xor ecx, ecx							; Clear Counter Register
		xor edx, edx							; Clear Data (Thread ID) Register
		.StrLen:
			lodsb								; Grab a character from Name Pointer
			or al, al							; Check for 0
			jz .FindName						; If 0 we have found length.
			inc ecx								; Else increment Counter Register
			jmp .StrLen							; Loop while *Name != 0
		.FindName:
			mov ebx, ecx						; Save Count in Base Register
			.IDLoop:
				mov esi, ebp					; Restore Name Pointer
				mov ecx, ebx					; Restore Count Register
				mov edi, edx					; Calculate Destination (Thread to test in EDX)
				shl edi, 9
				add edi, 4
				cmp DWORD [edi - 4], 0		; Check if we made it threw the list of threads.
				je .NotFound					; If we do, Fail.
				rep cmpsb						; compare Name
				je .IDDone						; If equal we found a match
				inc edx							; Else Increment Thread Number
				jmp .IDLoop						; Loop until complete or Fail
			.IDDone:
				mov edx, DWORD [esp - 4]
				mov DWORD [esp + 52], edx		; Store Thread ID in current threads eax
				ret								; return to current thread
		.NotFound:
			mov DWORD [esp + 52], 0				; Store 0 in current threads eax
			ret									; return to current thread
		ret

	.AddAPI:
xchg bx, bx
		; esi = function name pointer
		; ebx = function location
		mov edi, API_LIST_BASE
		.searchForOpening:
			cmp DWORD [edi], 0
			je .FoundOpening
			add edi, 0x200
			jmp .searchForOpening
		.FoundOpening:
			mov DWORD [edi], ebx
			add edi, 4
			.copyName:
				lodsb
				test al, al
				jz .Done
				mov BYTE [edi], al
				inc edi
				jmp .copyName
		.Done:
			mov BYTE [edi + 1], 0
			ret

	.InatallAPI:
xchg bx, bx
		; ecx = size in bytes
		; edi = Where To Make Buffer (in threads virutal space)
		; RETURN eax = Actual Virtual Address (API Base Location)
		;              So we know where to relocate it to.

		mov ebp, ecx
		shr ebp, 12
		test ecx, 0x00000FFF
		jz .PageCountDone
		inc ebp
		.PageCountDone:
			mov esi, API_CODE_TLB
			bt DWORD [esi], 0
			jnc .FoundBlankArea
			add esi, 4
			jmp .PageCountDone
		.FoundBlankArea:
			mov eax, esi ; store TLB Entry
			sub eax, DWORD [PageDirectoryBase] ; Subtract TLB Base
			sub eax, 0x1000 ; subtract for pdir
			shl eax, 10 ; multiply by 0x400
			mov [esp + 52], eax

			.AllocateArea:
				push 0x1000
				call _PMM_alloc
				or eax, 5
				mov DWORD [esi], eax
				add esp, 4

				pusha
				mov ecx, eax
				mov edx, edi
				xor eax, eax
				mov ebx, 1
				call _VMM_map_UNSAFE
				popa

				dec ebp
				jz .Done
				add edi, 0x1000
				add esi, 4
				jmp .AllocateArea
		ret
