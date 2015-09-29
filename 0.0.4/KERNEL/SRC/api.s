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

	cmp al, 0x01
	je .FindAPI
	cmp al, 0x02
	je .AddAPI
	cmp al, 0xF0
	je .InatallAPI

	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread.

	.FindAPI:
		; esi = function name pointer
		; RETURN eax = function location
		mov ebp, esi							; Save Pointer to Name
		xor ecx, ecx							; Clear Counter Register
		xor edx, edx							; Clear Data Register
		.StrLen:
			lodsb								; Grab a character from Name Pointer
			or al, al							; Check for 0
			jz .FindName						; If 0 we have found length.
			inc ecx								; Else increment Counter Register
			jmp .StrLen							; Loop while *Name != 0
		.FindName:
			mov ebx, ecx						; Save Count in Base Register
			mov edi, API_LIST_BASE				; Set Destination Pointer
			add edi, 4							; Set Destination Offset
			.IDLoop:
				mov esi, ebp					; Restore Name Pointer
				mov ecx, ebx					; Restore Count Register
				mov edx, DWORD [edi - 4]		; Save function pointer
				cmp edx, 0						; Check if we made it threw the list of APIs.
				je .NotFound					; If we do, Fail.
				push edi
				rep cmpsb						; compare Name
				pop edi
				je .IDDone						; If equal we found a match
				add edi, 0x20					; Else Destination Pointer
				jmp .IDLoop						; Loop until complete or Fail
			.IDDone:
				mov DWORD [esp + 52], edx		; Store API Function Pointer in current threads eax
				ret								; return to current thread
		.NotFound:
			mov DWORD [esp + 52], 0				; Store 0 in current threads eax
			ret									; return to current thread
		ret

	.AddAPI:
		; esi = function name pointer
		; ebx = function location
		mov edi, API_LIST_BASE					; Set Destination Pointer
		.searchForOpening:
			cmp DWORD [edi], 0					; Check for blank spot in API List
			je .FoundOpening
			add edi, 0x20						; Increment Destination Pointer
			jmp .searchForOpening
		.FoundOpening:
			mov DWORD [edi], ebx				; Store Function Pointer in API List
			add edi, 4							; Add offset to name in API List
			.copyName:
				lodsb
				test al, al
				jz .Done
				mov BYTE [edi], al
				inc edi
				jmp .copyName
		.Done:
			mov BYTE [edi + 1], 0				; NULL Terminate Name
			ret

	.InatallAPI:
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
			.FindBlank:
				bt DWORD [esi], 0
				jnc .FoundBlankArea
				add esi, 4
				jmp .FindBlank
		.FoundBlankArea:
			mov eax, esi ; store TLB Entry
			sub eax, DWORD [PageDirectoryBase] ; Subtract TLB Base
			sub eax, 0x1000 ; subtract for pdir
			shl eax, 10 ; multiply by 0x400
			mov [esp + 52], eax

			.AllocateArea:
				pusha
				push edi
				; push esi
				push 0x1000
				call _PMM_alloc
				add esp, 4
				; pop esi


				mov edx, [esp+88]
				push eax
				mov ecx, eax
				xor eax, eax
				xor ebx, ebx
				call _VMM_map_UNSAFE
				pop eax

				pop edi
				mov ecx, eax
				mov edx, edi
				xor eax, eax
				mov ebx, 1
				call _VMM_map_UNSAFE
				popa

				dec ebp
				jz .Allocated
				add edi, 0x1000
				add esi, 4
				jmp .AllocateArea
	.Allocated:
		ret
