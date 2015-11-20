%include "./include/bos/bootinf.inc"

[global TotalRAM]
[global FreeRAM]
[global UsedRAM]
[global _PMM_init]
[global _PMM_alloc]
[global _PMM_free]

[extern PMMBase]




SECTION .text ; Code area, Only Executable code please
align 0x00001000

[bits 32]
PageSize equ 0x1000
PageBitShift equ 12

_PMM_init:
	mov esi, DWORD [BOOTINF.MMapAddr]						; Memory Map Address
	movzx ecx, WORD [BOOTINF.MMapEnts]						; Memory Map Entries
	.EntryLoop:
		push ecx
		mov edx, DWORD [esi + 0x0C]							; High 32-bits of Memory Length
		mov eax, DWORD [esi + 0x08]							; Low 32-bits of Memory Length
		call _ConvertLongAddr								; Convert EDX:EAX to page number (EAX)
		mov ecx, eax										; Move Page Number to Page Length (ECX)
		shl eax, 2											; Convert Page Number to KBs
		add DWORD [TotalRAM], eax							; Increase TotalRAM
		add DWORD [UsedRAM], eax							; Increase UsedRAM
		mov eax, DWORD [esi + 0x10]							; Memory Type
		cmp eax, 1											; Is it Free?
		jne .NextEntry										; No, Go to NextEntry
		mov edx, DWORD [esi + 0x04]							; High 32-bits of Memory Base
		mov eax, DWORD [esi]								; Low 32-bits of Memory Base
		test eax, 0x00000FFF								; Check if we are Aligned
		jz .ContinueEntry									; If so this value is good
		add eax, PageSize									; If not add PageSize
		jnc .ContinueEntry									; If Carry Flag is not set Continue
		inc edx												; Else increase EDX
		.ContinueEntry:
			call _ConvertLongAddr							; Convert EDX:EAX to page number (EAX)
			movzx ebx, BYTE [BOOTINF.ModCount]				; Move ModCount to EBX
			test ebx, ebx									; Do we have mods loaded?
			jz .OnlyKernel									; No, We only have Kernel
			shl ebx, 3										; else ModCount *= 8
			sub ebx, 4										; ModCount -= 4
			mov edx, [MODLIST + ebx]						; Get End of last Mod (EDX)
			jmp .Continue									; Skip Kernel Only Section
			.OnlyKernel:
				mov edx, DWORD [BOOTINF.KernelEnd]			; Get End of Kernel (EDX)
			.Continue:
				mov ebx, DWORD [BOOTINF.KernelStart]		; Get KernelStart (EBX)
				shr ebx, PageBitShift						; KernelStart >> PageBitShift (12)
				test edx, 0xFFF								; Check if End Of Data (EDX) is Aligned
				jz .Aligned									; If so skip next instruction
				add edx, PageSize							; If not, End of Data (EDX) += PageSize
				.Aligned:
					shr edx, PageBitShift					; End Of Data >> PageBitShift (12)
				cmp eax, 0x80								; Below first 512KB?
				jb .LowMemFix								; Yes, Fix to reserve first 512KB.
				cmp eax, edx								; Is Base < End Of Data?
				jae .ContinueFree							; No, Free This Entry
				cmp eax, ebx								; Is Base >= KernelStart?
				jae .HighMemFix								; Yes, Fix Base for after the Kernel
			.FreeEntry:
				mov ebp, eax								; EBP = Base
				add ebp, ecx								; EBP += Length
				cmp ebp, ebx								; Is Base + Length < KernelStart
				jb .ContinueFree							; Yes, Free it.
				sub ebp, ebx								; EBP -= KernelStart
				sub ecx, ebp								; Length -= EBP
				.ContinueFree:
					call _PMM_free							; Free Memory
		.NextEntry:
			add esi, 0x18									; Memory Entries are 24 bytes (0x18)
			pop ecx
			dec ecx											; Decrement Counter
			jnz .EntryLoop									; Contine if Counter ! 0
		ret													; Return
		.LowMemFix:
			mov ebp, 0x80									; EBP = ReservedPages
			sub ebp, eax									; EBP -= BasePage
			add eax, ebp									; BasePage += EBP
			sub ecx, ebp									; PageLength -= EBP
			jmp .FreeEntry									; Continue
		.HighMemFix:
			mov ebp, edx									; EBP = End Of Data
			sub ebp, eax									; EBP -= BasePage
			add eax, ebp									; BasePage += EBP
			sub ecx, ebp									; PageLength -= EBP
			jmp .ContinueFree								; Free the Page.

; ecx - length in pages (4kb)
; RETURN edx:eax - Physical Base
_PMM_alloc:
	call LockPMM
	pushad													; Save Registers
	xor edx, edx											; Clear EDX
	mov esi, PMMBase										; ESI = Memory Stack
	mov ebp, DWORD [PMMEntries]								; Get Current Entry Count
	test ebp, ebp											; Do we have entries?
	jz OUT_OF_RAM											; If not we are OUT_OF_RAM
	.EntryLoop:
		cmp DWORD [esi + 4], ecx							; Check Length
		jge .FoundSpace										; if >= requested we are done
		add esi, 8											; Length of Entry is 64-bit (8 byte)
		dec ebp												; Dec Entry Count
		jnz .EntryLoop										; Loop If more Entries
		jmp OUT_OF_RAM										; If we get here we are OUT_OF_RAM
	.FoundSpace:
		mov eax, DWORD [esi]								; Save old base in EAX
		add DWORD [esi], ecx								; Increase Base by requested Length
		sub DWORD [esi + 4], ecx							; Decrease Length by requested Length
		shl ecx, 2											; Length to KBs
		add DWORD [UsedRAM], ecx							; Increase UsedRAM
		sub DWORD [FreeRAM], ecx							; Decrease FreeRAM
		test eax, 0x00F00000								; Is this high memory (>4GB)
		jz .Continue										; No Continue
		mov edx, eax										; copy eax to edx
		shr edx, 20											; bitshift high by 20
	.Continue:
		shl eax, PageBitShift								; bitshift left by 12
		mov [esp + 0x18], edx								; save EDX for the popad
		mov [esp + 0x20], eax								; save EAX for the popad
	popad													; Restore Registers
	jmp UnlockPMM											; Unlock PMM and Return

; eax - First Page
; ecx - length in pages
_PMM_free:
	call LockPMM
	pushad													; Save Registers
	mov esi, PMMBase										; ESI = Memory Stack
	mov ebp, DWORD [PMMEntries]								; Get Current Entry Count
	test ebp, ebp											; Do we have entries?
	jz .AddEntry											; No, Add one
	.EntryLoop:
		mov ebx, DWORD [esi]								; Get Base Page Number
		mov edx, DWORD [esi + 4]							; Get Page Count
		mov edi, ebx										; EDI = Count
		add edi, edx										; EDI += Base
		cmp edi, eax										; if (Count + Base) = FirstPage
		je .AboveEntry										; Add Above Entry
		mov edi, eax										; EDI = FirstPage
		add edi, ecx										; EDI += Length
		cmp edi, ebx										; if (FirstPage + Length) = Base
		je .BelowEntry										; Add Below Entry
		.ContinueEntryLoop:
			add esi, 8										; Length of Entry is 64-bit (8 Bytes)
			dec ebp											; Dec Entry Count
			jnz .EntryLoop									; If more Entries Loop
	.AddEntry:
		inc DWORD [PMMEntries]								; Increase Total Memory Entry Count
		mov DWORD [esi], eax								; Set Base to FirstPage
		mov DWORD [esi + 4], ecx							; Set Count to Length
		jmp .FixVariables									; Continue
	.BelowEntry:
		sub DWORD [esi], ecx								; Subtract Length from Base
		jmp .FixVariables									; Continue
	.AboveEntry:
		add DWORD [esi + 4], ecx							; Add Length to Count
	.FixVariables:
		shl ecx, 2											; Pages To KBs
		add DWORD [FreeRAM], ecx							; Increase FreeRAM
		sub DWORD [UsedRAM], ecx							; Decrease UsedRAM
	popad													; Restore Registers
	jmp UnlockPMM											; Unlock PMM and Return

;EDX:EAX = 64-bit address
;RETURN EAX = 24-bit Page Number
_ConvertLongAddr:
	test edx, 0xFFFFFFF0									; Check to see if Memory is above 36-bit limit
	jz .Continue											; If not, continue
	mov edx, 0x0000000F										; If is, Set high 4 bits to 1
	mov eax, 0xFFFFFFFF										; And low 32 bits to 1
	.Continue:
		shr eax, PageBitShift								; Shift Low 32 bits right PageBitShift
		shl edx, 20											; Shift high 4 bits left 20 spaces
		or eax, edx											; Or together to make a 24-bit virtual address
		ret													; Return

OUT_OF_RAM:													; One Day I may do something other than DIE...
	cli
	hlt
	jmp OUT_OF_RAM

LockPMM:
	push eax												; Save Registers
	push ecx
	mov ecx, 1												; Locked = 1
	xor eax, eax											; UnLocked = 0
	.AquireLock:
		lock cmpxchg [PMMLOCK], ecx							; If PMMLOCK is Unlocked, Lockit
		jz .HaveLock										; If it locked, return
		pause												; Notify CPUs we are spinning
		jmp .AquireLock										; Spin
	.HaveLock:
		pop ecx												; Restore Registers
		pop eax
		ret													; Return

UnlockPMM:
	mov DWORD [PMMLOCK], 0									; Clear the Lock
	ret														; Return




SECTION .data ; Data Located in the ELF
align 0x00001000

TotalRAM dd 0
UsedRAM dd 0
FreeRAM dd 0
PMMEntries dd 0

PMMLOCK dd 0
