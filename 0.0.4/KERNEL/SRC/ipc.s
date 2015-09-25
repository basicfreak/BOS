[bits 32]

[global ClearMessageSystem]
[global SendMessage]
[global IPC_Handler]

[extern _VMM_umap_UNSAFE]
[extern _VMM_map_UNSAFE]
[extern _VMM_getPhysOther]
[extern _VMM_mapOther]
[extern PageDirectoryBase]
[extern _PMM_free]
[extern TaskList]
[extern CurrentThread]
[extern LoadThread]
[extern ThreadManager]
[extern INTList]
[extern TF_ACTIVE]
[extern TF_USESSE]
[extern TF_WAITINT]
[extern TF_DEAD]
[extern TF_WAITMSG]
[extern TF_BLANK]
[extern TF_REGS]
[extern TF_LINKF]

MSGSYS_BASE equ 0xD0000000
MSGSYS_PAGETABLES equ 0x60
MSGSYS_PAGEENTRIES equ 0x10000

MSGSYS_PDIR_BASE equ 0xFFBFFD00
MSGSYS_PTBL_BASE equ 0xFFF40000
MSGSYS_TLB_BASE equ 0xFFFFFD00

IPC_Handler:
	mov eax, DWORD [esp + 52]					; Grab Function Number (stored in eax of the calling thread)
	cmp al, 0x01								; Wait for message?
	je .WaitMSG									; Yes? Handle Flags.
	cmp al, 0x02								; Send a message?
	je SendMessage								; Yes? Send Message.
	cmp al, 0x80								; Wait for IRQ/INT?
	je .WaitINT									; Yes? Handle Flags.
	cmp al, 0x81								; Wait for IRQ/INT with regs?
	je .WaitINTwr								; Yes? Handle Flags.
	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread
	.WaitMSG:
;xchg bx, bx
;push 0x01020304
;add esp, 4
		shl edx, 10
		mov ebp, DWORD [CurrentThread]			; CurrentThread ID
		shl ebp, 10								; Multiply by 1KB
		add ebp, DWORD [TaskList]				; Add ThreadList Base
		and DWORD [ebp + 76], TF_DEAD			; Clear Active Flag
		or DWORD [ebp + 76], TF_WAITMSG			; Set Wait For MSG Flag
		or DWORD [ebp + 76], edx				; Set Thread ID we are waiting on.
		jmp ThreadManager
	.WaitINTwr:
		mov edx, DWORD [CurrentThread]			; CurrentThread ID
		shl edx, 10								; Multiply by 1KB
		add edx, DWORD [TaskList]				; Add ThreadList Base
		or DWORD [edx + 76], TF_REGS			; Set Regs Flag
	.WaitINT:
		mov eax, DWORD [esp + 40]				; Threads EBX (int_no we are waiting for)
		shl eax, 2								; Multiply int_no by 4 (4 bytes per DWORD)
		add eax, DWORD [INTList]				; Add base address for int list to int_no
		mov edx, [eax]							; Grab thread ID or Flag
		cmp edx, 0xFFFFFFFF						; Has this INT Fired Yet?
		je .FiredAlready						; Yes?
		mov edx, DWORD [CurrentThread]			; CurrentThread ID
		mov [eax], edx							; Save CurrentThread ID in INT Table.
		shl edx, 10								; Multiply by 1KB
		add edx, DWORD [TaskList]				; Add ThreadList Base
		and DWORD [edx + 76], TF_DEAD			; Clear Active Flag
		or DWORD [edx + 76], 0x00000004			; Set Wait for INT Flag
		jmp ThreadManager						; Yeild The Thread if INT not ready.
	.FiredAlready:
		mov DWORD [eax], 0						; Clear Fired Flag
		ret										; Return to Thread




ClearMessageSystem:
;xchg bx, bx
;push 0x87654321
;add esp, 4

; EAX and EDX must be preserved!!!!!
; ALL OTHERS CAN BE TRASHED.

; 
; There are up to 0x40 Page Tables with 0x10000 Entries (0x400 each Table)
; The Directory Entries Are 0xFFBFFD00 - 0xFFBFFDFC (and TBL is + 0x400000)
; The Page Entries Are 0xFF400000 - 0xFF800000
; 

	push eax
	push edx

	mov edi, MSGSYS_PDIR_BASE
	mov esi, MSGSYS_PTBL_BASE
	mov ebp, MSGSYS_TLB_BASE

	.TBLLoop:
		bt DWORD [ebp], 0 						; Check to see if current Page Table is present
		jnc .Done								; If not we should be done here.
		xor ebx, ebx							; Clear ebx
		.EntLoop:
			bt DWORD [esi + ebx], 0				; Check is Page Entry is present
			jnc .Done							; If not we should be done here.
			mov edx, DWORD [esi + ebx]			; Copy Page Entry
			and edx, 0xFFFFF000					; Grab Physical Address From Page Entry.
			push 0x1000							; Push Page Size
			push edx							; Push Physical Address
			call _PMM_free						; Tell PMM To Free Page.
			add esp, 8							; Clean Stack.
			mov DWORD [esi + ebx], 0			; Clear Page Entry
			add ebx, 4							; Increase Offset By 4
			cmp ebx, 0x1000						; Check End Of Page Table
			jl .EntLoop							; If Not Continue Loop
		mov DWORD [edi], 0						; Clear Page Tables Entry
		mov DWORD [ebp], 0						; Clear TLB Entry
		invlpg [esi]							; Invalidate the Table
		add esi, 0x1000							; Increase Table Base By 0x1000
		add edi, 4								; Increase Dir Base by 4
		add ebp, 4								; Increase TLB Base by 4
		cmp ebp, 0xFFFFFDFC						; Did we pass the last table?
		jle .TBLLoop							; Continue the loop Until Done.
	.Done:
		pop edx
		pop eax
	ret

SendMessage:
;xchg bx, bx
;push 0x43215678
;add esp, 4

	; EDX = Destination ID
	; IF EDX == 0 ESI = Destination Name
	test edx, edx
	jnz .HaveThreadID
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
				shl edi, 10						; Multiply Destination by 1024
				add edi, DWORD [TaskList]		; Add Base of Task List
				cmp DWORD [edi + 76], TF_BLANK	; Check if we made it threw the list of threads.
				je .NotFound					; If we do, Fail.
				add edi, 84						; Add offset to Name in Thread
				rep cmpsb						; compare Name
				je .HaveThreadID				; If equal we found a match
				inc edx							; Else Increment Thread Number
				jmp .IDLoop						; Loop until complete or Fail
		.NotFound:
			xchg bx, bx
			push 0xDEADBEEF
			add esp, 4
			mov DWORD [esp + 52], 0				; Store 0 in current threads eax
			ret									; return to current thread
	.HaveThreadID:
		mov edi, edx
		shl edi, 10
		add edi, DWORD [TaskList]
		mov ebx, DWORD [edi + 76]
		bt ebx, 3								; Is this thread waiting on a message
		jnc .NotFound
		shr ebx, 10
		test ebx, ebx							; Does this thread care who sends a message?
		jz .Send
		cmp ebx, DWORD [CurrentThread]			; Is the tread waiting on CurrentThread?
		jne .NotFound
		.Send:
			push edx
			mov eax, DWORD [edi + 80]			; Store CR3 of Destination

;void *_VMM_getPhysOther(void* PDIR, void* Virt)
			push 0xD0000000
			push eax
			call _VMM_getPhysOther
			test eax, eax
			jnz .ClearDestination
			pop eax
			add esp, 4

			mov edi, MSGSYS_PDIR_BASE
			mov esi, MSGSYS_PTBL_BASE
			mov ebp, MSGSYS_TLB_BASE
			mov ecx, MSGSYS_BASE
			.TBLLoop:
				bt DWORD [ebp], 0 				; Check to see if current Page Table is present
				jnc .Done						; If not we should be done here.
				xor ebx, ebx					; Clear ebx
				.EntLoop:
					bt DWORD [esi + ebx], 0		; Check is Page Entry is present
					jnc .Done					; If not we should be done here.
					mov edx, DWORD [esi + ebx]	; Copy Page Entry
					and edx, 0xFFFFF000			; Grab Physical Address From Page Entry.

;void _VMM_mapOther(void* PDIR, void* Virt, void* Phys, bool User, bool Write)
					push 0
					push 1
					push edx
					push ecx
					push eax
					call _VMM_mapOther
					pop eax
					pop ecx
					pop edx
					add esp, 8

					mov DWORD [esi + ebx], 0	; Clear Page Entry
					add ebx, 4					; Increase Offset By 4
					add ecx, 0x1000
					cmp ebx, 0x1000				; Check End Of Page Table
					jl .EntLoop					; If Not Continue Loop
				mov DWORD [edi], 0				; Clear Page Tables Entry
				mov DWORD [ebp], 0				; Clear TLB Entry
				invlpg [esi]					; Invalidate the Table
				add esi, 0x1000					; Increase Table Base By 0x1000
				add edi, 4						; Increase Dir Base by 4
				add ebp, 4						; Increase TLB Base by 4
				cmp ebp, 0xFFFFFDFC				; Did we pass the last table?
				jle .TBLLoop					; Continue the loop Until Done.
	.Done:
		pop edx
		mov edi, edx
		shl edi, 10
		add edi, DWORD [TaskList]
		and DWORD [edi + 76], 0x3FB
		or DWORD [edi + 76], 1
		jmp LoadThread

	.ClearDestination:
	;xchg bx, bx
		pop eax
		add esp, 4

		mov edx, cr3
		mov cr3, eax
		call ClearMessageSystem
		mov cr3, edx

		mov edi, MSGSYS_PDIR_BASE
		mov esi, MSGSYS_PTBL_BASE
		mov ebp, MSGSYS_TLB_BASE
		mov ecx, MSGSYS_BASE
		jmp .TBLLoop