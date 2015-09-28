[bits 32]

[global getPageDir]
[global setPageDir]
[global getCR2]
[global PageDirectoryBase]
[global MEM_Handler]
[global _VMM_PageFaultHandler]
[global _VMM_umap_UNSAFE]
[global _VMM_map_UNSAFE]

[extern killCurrentThread]
[extern KILL_SYSTEM]
[extern _PMM_alloc]
[extern _VMM_map]
[extern _VMM_getPhys]
[extern _VMM_newPDir]
[extern _VMM_mapOther]
[extern _VMM_getPhysOther]
[extern _PMM_free]
[extern _VMM_umap]
[extern _VMM_umapOther]
[extern ClearMessageSystem]
[extern TF_ACTIVE]
[extern TF_USESSE]
[extern TF_WAITINT]
[extern TF_DEAD]
[extern TF_WAITMSG]
[extern TF_BLANK]
[extern TF_REGS]
[extern TF_LINKF]

getPageDir:
	mov eax, cr3
	ret

setPageDir:
	push eax
	mov eax, [esp + 8]
	mov cr3, eax
	pop eax
	ret

getCR2:
	mov eax, cr2
	ret


; NOTE: These are only to be used by page fault and thread memory handlers...
; NOT C COMPLIENT!!!

MEM_Handler:
	mov eax, DWORD [esp + 52]					; Grab Function Number (stored in eax of the calling thread)
	cmp al, 0x00								; Are we requesting new page?
	je .AllocMap								; Yes? Allocate and Map Page.
	cmp al, 0x01								; Are we unmapping and freeing page?
	je .FreeUMap								; Yes? Free And Unmap Page.
	cmp al, 0x80								; Are We mapping a known address?
	je .Map										; Yes? Map Page.
	cmp al, 0x81								; Are we unmapping a page?
	je .UMap									; Yes? Unmap page.
	cmp al, 0x8F								; Find Physical Address?
	je .FindPhys								; Yes? Find Address.
	cmp al, 0xF0								; Create new Dir?
	je .NewDir									; Yes? Make Dir.
	cmp al, 0xF1								; Are We mapping a known address on another pdir?
	je .MapOther								; Yes? Map page in pdir.
	cmp al, 0xF2								; Are we requesting new page on another pdir?
	je .AllocMapOther							; Yes? Allocate and Map page for pdir.
	cmp al, 0xF3								; Are we unmapping a page in another pdir?
	je .UMapOther								; Yes? Unamp page in pdir.
	cmp al, 0xF4								; Are we unmapping and freeing a page in another pdir?
	je .FreeUMapOther							; Yes? Free and Unmap Page in pdir.
	cmp al, 0xFF								; Are we requesting physical address in another pdir?
	je .FindPhysOther							; Yes? Get physical address from pdir.
	mov DWORD [esp + 52], 0						; ELSE set threads eax to 0
	ret											; Return to thread

	.AllocMap:
;	_VMM_map((void*)r->edx, _PMM_alloc(PAGESIZE), TRUE, (bool)r->ebx);
		push DWORD 0x1000						; Push PAGESIZE
		call _PMM_alloc							; Call Physical Memory Allocator
		mov ecx, eax							; Store address in ecx
		pop eax									; Clean Stack
	.Map:
;	_VMM_map((void*)r->edx, (void*)r->ecx, TRUE, (bool)r->ebx);
		;push ebx								; Push Write Flag
		;push DWORD 1							; Push TRUE, this is a user page
		;push ecx								; Push Physical Address
		;push DWORD [esp + 56]					; push r->edx (this is changed if page is allocated)
		;call _VMM_map							; Map Page
		;add esp, 16								; Clean Stack
		;eax KERNEL Flag
		;ecx Physical
		;edx Virtual
		;ebx WriteFlag
		xor eax, eax
		mov edx, DWORD [esp + 44]
		call _VMM_map_UNSAFE
		ret										; Return to thread

	.FreeUMap:
;	_PMM_free(_VMM_getPhys((void*)r->edx), PAGESIZE);
		push edx								; Push Virutal Address r->edx
		call _VMM_getPhys						; Call Get Physical Address Function (returns eax)
		push DWORD 0x1000						; Push PAGESIZE
		push eax								; Push Physical Address
		call _PMM_free							; Free The Page
		add esp, 8								; Clean Stack
		pop edx
	.UMap:
;	_VMM_umap((void*)r->edx);
		;push DWORD [esp + 44]					; Push Virutal Address r->edx (changed if we free the page first)
		;call _VMM_umap							; unmap the page.
		;pop eax									; Clean Stack
		call _VMM_umap_UNSAFE
		ret										; Return to thread

	.FindPhys:
;	r->eax = (uint32_t) _VMM_getPhys((void*)r->edx);
		push edx								; Push Virtual Address r->edx
		call _VMM_getPhys						; Get Physical Address
		mov DWORD [esp + 56], eax				; Save Physical Address in r->eax
		pop eax									; Clean Stack
		ret										; Return to thread

	.NewDir:
;	r->eax = (uint32_t) _VMM_newPDir();
		call _VMM_newPDir						; Get New Page Directory
		mov DWORD [esp + 52], eax				; Save Page Directory Address in r->eax
		ret										; Return to thread

	.AllocMapOther:
;	_VMM_mapOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx, _PMM_alloc(PAGESIZE), TRUE, (bool) r->ebx);
		push eax								; Save eax
		push DWORD 0x1000						; Push Page Size
		call _PMM_alloc							; Allocate Page
		mov ecx, eax							; Save Page in ecx
		pop eax									; Clean Stack
		pop eax									; Restore eax
	.MapOther:
;	_VMM_mapOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx, (void*) r->ecx, TRUE, (bool) r->ebx);
		and eax, 0xFFFFF000						; Remove Function Number From eax
		push ebx								; Push Write Flag
		push DWORD 1							; Push TRUE (user flag)
		push ecx								; Push Physical Address
		push DWORD [esp + 56]					; Push Virtual Address r->edx ( as this is changed if we allocated )
		push eax								; Push Page Directory Address
		call _VMM_mapOther						; Map Page
		add esp, 20								; Clean Stack
		ret										; Return to thread

	.FreeUMapOther:
;	_PMM_free(_VMM_getPhysOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx), PAGESIZE);
	.UMapOther:
;	_VMM_umapOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx);
		ret

	.FindPhysOther:
;	r->eax = (uint32_t) _VMM_getPhysOther((void*) (r->eax & 0xFFFFF000), (void*) r->edx);
		and eax, 0xFFFFF000						; Remove Function Number From eax
		push edx								; Push Virtual Address
		push eax								; Push Page Directory Address
		call _VMM_getPhysOther					; Get Physical Address
		mov DWORD [esp + 60], eax				; Save Physical Address in r->eax
		add esp, 8								; Clean Stack
		ret										; Return to thread

_VMM_PageFaultHandler:
;xchg bx, bx
	; Should only get here in user mode.
	mov eax, DWORD [esp + 60]					; Store Error Code r->err_code
	mov edx, cr2								; Store Fault Address cr2
	and edx, 0xFFFFF000							; Change to beginning of page.

	bt eax, 2									; Is User Bit Set?
	jnc KILL_SYSTEM								; No? Kernel Page Fault, Kill System.

	cmp edx, 0xFF000000							; Is the fault in Kernel Space?
	jge killCurrentThread						; Yes? Kill Current Thread

	cmp edx, 0xE4110000							; API ISSUE
	jge killCurrentThread

	cmp edx, 0xE0000000							; ThreadMan ISSUE
	jge .expandThreadMan

	cmp edx, 0xD0000000							; Is the fault in IPC Message Area?
	jge .MessageSystem							; Yes? Go to Message Area Handler

	cmp edx, 0xCF800000							; Is the fault in the User Stack?
	jge .StackSpace								; Yes? Go to Stack Space Handler.

	bt eax, 1									; Is this a write error?
	jnc killCurrentThread						; No? Kill current thread. We do not know what to do.

	bt eax, 0									; Is this page present?
	jnc killCurrentThread						; No? Kill current thread. We do not know what to do.

	.CopyOnWrite:

		push edx
		push 0x1000
		call _PMM_alloc							; Allocate a new page
		mov ecx, eax
		mov ebx, 1
		mov eax, 1
		xor edx, edx
		call _VMM_map_UNSAFE
		pop edx
		pop edx
		push ecx

		xor edi, edi
		mov esi, edx
		mov ecx, 0x400
		rep movsd								; Copy Data from read-only page.
		pop ecx
		xor eax, eax
		mov ebx, 1

		call _VMM_map_UNSAFE
		xor edx, edx
		call _VMM_umap_UNSAFE
		ret										; Return to thread.

	.StackSpace:
		bt eax, 0								; Is the page present?
		jc .CopyOnWrite							; Yes? This is a Copy On Write.
		push edx
		push 0x1000
		call _PMM_alloc							; Allocate a new page.
		mov ecx, eax

		xor eax, eax
		mov ebx, 1
		pop edx
		pop edx
		call _VMM_map_UNSAFE
		mov edi, edx

		mov ecx, 0x400
		jmp .clearmem

	.MessageSystem:
;xchg bx, bx
;push 0x12345678
;add esp, 4

		bt eax, 1
		jnc killCurrentThread					; Thread attempted to read non present message

		bt eax, 0
		jnc .msgsysmap
		call ClearMessageSystem
		.msgsysmap:
			push edx
			push 0x1000
			call _PMM_alloc						; Allocate a new page.
			mov ecx, eax

			xor eax, eax
			mov ebx, 1
			pop edx
			pop edx
			call _VMM_map_UNSAFE
			mov edi, edx

			mov ecx, 0x400
		.clearmem:
			mov DWORD [edi], 0
			add edi, 4
			dec ecx
			jnz .clearmem
		ret

	.expandThreadMan:
		bt eax, 0
		jc KILL_SYSTEM
		
		push edx
		push 0x1000
		call _PMM_alloc
		
		test eax, eax
		jz KILL_SYSTEM

		mov ecx, eax
		mov eax, 1
		mov ebx, 1
		pop edx
		pop edx
		call _VMM_map_UNSAFE
		add edx, 76
		mov DWORD [edx], 0xFF00FF00
		add edx, 0x400
		mov DWORD [edx], 0xFF00FF00
		add edx, 0x400
		mov DWORD [edx], 0xFF00FF00
		add edx, 0x400
		mov DWORD [edx], 0xFF00FF00
		ret

_VMM_map_UNSAFE:
	;eax KERNEL Flag
	;ecx Physical
	;edx Virtual
	;ebx WriteFlag
	mov ebp, edx
	shr ebp, 22
	shl ebp, 2
	mov edi, DWORD [PageDirectoryBase]
	add edi, ebp
	bt DWORD [edi], 0
	jc .MapPage

	push eax
	push edx
	push ecx
	push 0x1000
	call _PMM_alloc
	pop edx
	pop ecx
	or eax, 1
	test ebx, ebx
	jz .ReadOnly
	or eax, 2
	.ReadOnly:
	mov DWORD [edi], eax
	mov DWORD [edi + 0x400000], eax
	mov edx, edi
	sub edx, DWORD [PageDirectoryBase]
	shl edx, 12
	add edx, DWORD [PageDirectoryBase]
	invlpg [edx]
	pop edx
	pop eax
	test eax, eax
	jnz .KernelOnly
	or DWORD [edi], 4
	.KernelOnly:

	.MapPage:
	bt DWORD [edi + 0x400000], 0
	jc .contmap
	mov ebp, DWORD [edi]
	mov DWORD [edi + 0x400000], ebp
	.contmap:
	push edi
	sub edi, DWORD [PageDirectoryBase]
	shr edi, 2
	;page table number in edi
	inc edi
	shl edi, 12
	add edi, DWORD [PageDirectoryBase]

	mov esi, edx
	and esi, 0x3FF000
	shr esi, 12
	shl esi, 2
	add esi, edi
	pop edi
	test eax, eax
	jnz .KOnly
	or ecx, 4
	bt DWORD [edi], 2
	jc .KOnly
	or DWORD [edi], 4
	.KOnly:
	test ebx, ebx
	jz .ROnly
	or ecx, 2
	bt DWORD [edi], 1
	jc .ROnly
	or DWORD [edi], 2
	.ROnly:
	or ecx, 1

	mov DWORD [esi], ecx
	invlpg [edx]
	ret

_VMM_umap_UNSAFE:
	mov edi, edx
	shr edi, 22
	inc edi
	shl edi, 12
	add edi, DWORD [PageDirectoryBase]
	mov esi, edx
	and esi, 0x3FF000
	shr esi, 12
	shl esi, 2
	add esi, edi
	mov DWORD [esi], 0
	invlpg [edx]
	ret

PageDirectoryBase dd 0xFFBFF000