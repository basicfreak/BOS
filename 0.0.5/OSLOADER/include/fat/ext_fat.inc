%ifndef __TFTP_INC__INCLUDED__
%define __TFTP_INC__INCLUDED__
%ifndef __EXT_FAT_INC__INCLUDED__
%define __EXT_FAT_INC__INCLUDED__

[bits 16]

; ds:si = file path
; edx = destination
ReadFile:
	pushad										; Save Registers
	mov DWORD [FileSize], 0						; Reset FileSize
	push si										; puts(LOADMSG)
	mov si, LOADMSG
	call puts
	pop si
	call puts									; puts(si)
	push edx									; Save Destination
	call ConvertPath							; Convert the path to something useful.
	mov si, FilePath							; Move ConvertedPath location to si
	call FAT_FindFile_Root						; Find the first FileName in the Root Directory
	jc ERROR									; If not found ERROR
	mov bp, WORD [PathDepth]					; DepthCounter = PathDepth
	dec bp										; DepthCounter --
	test bp, bp									; Did we find the last FileName?
	jz .FoundFile								; Yes.
	.SubDirLoop:
		add si, 11								; ConvertedPath += 11
		call FAT_FindFile_Dir					; Find File in sub directory
		jc ERROR								; If not found ERROR
		dec bp									; DepthCounter --
		test bp, bp								; Did we find the last FileName?
		jnz .SubDirLoop							; No, loop
	.FoundFile:
		mov di, 0x2000							; File Found, Temp buffer is 0x2000
		call FAT_ReadCluster					; Read cluster
		mov ecx, eax							; Copy ClusterSize to ecx
		add DWORD [FileSize], eax				; FileSize += ClusterSize
		pop edi									; Restore Destination
		mov esi, DWORD 0x2000					; Temp Buffer = 0x2000
		call memcpy								; Copy from Temp Buffer to Destination
		push edi								; Save Destination (which was incremented by memcpy)
		mov si, WAITMSG							; puts(WAITMSG)
		call puts
		cmp WORD [CurrentCluster], 0			; Did something F*** up?
		je ERROR								; If so ERROR!
		cmp WORD [CurrentCluster], 0xFFF0		; Did we read the last Cluster?
		jb .FoundFile							; If not Loop
	pop edx										; Restore Destination
	mov si, DONEMSG								; puts(DONEMSG)
	call puts
	popad										; Restore Registers
	mov eax, DWORD [FileSize]					; FileSize returns in eax
	ret											; Return

FAT_FindFile_Dir:
	pusha										; Save Registers
	.ClusterLoop:
		mov di, 0x2000							; Temp Buffer = 0x2000
		call FAT_ReadCluster					; Read Cluster
		shr eax, 4								; Entries = (ClusterSize / 16)
		mov di, 0x2000							; EntryLocation = 0x2000
		.DirLoop:
			push si								; Save FileName
			push di								; Save EntryLocation
			mov cx, 11							; FileName Length = 11
			rep cmpsb							; Compare FileName
			pop di								; Restore EntryLocation
			pop si								; Restore FileName
			je .Found							; Did We Find It?
			add di, 0x20						; EntryLocation += 32
			dec eax								; Entries --
			jnz .DirLoop						; if(Entries) loop
		cmp WORD [CurrentCluster], 0			; Did something F*** up?
		je ERROR								; If so ERROR!
		cmp WORD [CurrentCluster], 0xFFF0		; Did we read the last Cluster?
		jb .ClusterLoop							; If not Loop.
	.NotFound:
		mov WORD [CurrentCluster], 0			; File Not Found, Set CurrentCluster to 0
		popa									; Restore Registers
		stc										; Set Carry Flag
		ret										; Return
	.Found:
		mov ax, WORD [di + 0x001A]				; FirstCluster = WORD [EntryLocation + 0x1A]
		mov WORD [CurrentCluster], ax			; CurrentCluster = FirstCluster
		popa									; Restore Registers
		clc										; Make sure Carry Flag is clear
		ret										; Return


; I'm Not Commenting this as I am going to replace it ASAP with a much more efficient version...
ConvertPath:
	pusha
	xor bp, bp ; use bp to get depth
	mov di, FilePath
	lodsb
	cmp al, '/'
	je .PathLoop
	dec si
	.PathLoop:
		inc bp
		mov cx, 8
		.Name:
			lodsb
			cmp al, '.'
			je .Extention
			cmp al, '/'
			je .DepthSoon
			cmp al, 0
			je .Finished

			cmp al, 'a'
			jl .NameSave
			sub al, ' '
			.NameSave:
				stosb
				dec cx
				jnz .Name
		.Extention:
			mov al, 0x20
			rep stosb
			mov cx, 3
			lodsb
			cmp al, '.'
			je .ExtLoop
			dec si
			.ExtLoop:
				lodsb
				cmp al, '/'
				je .Depth
				cmp al, 0
				je .Finished
				cmp al, '.'
				je ERROR

				cmp al, 'a'
				jl .ExtSave
				sub al, ' '
				.ExtSave:
					stosb
					dec cx
					jnz .ExtLoop
		.Depth:
			test cx, cx
			jnz .DepthCont
			lodsb
			.DepthCont:
				cmp al, '/'
				jne .Finished
				mov al, 0x20
				rep stosb
				cmp bp, 8
				jb .PathLoop
				jmp ERROR
		.DepthSoon:
			mov al, 0x20
			add cx, 3
			rep stosb
			cmp bp, 8
			jb .PathLoop
			jmp ERROR
	.Finished:
		mov al, 0x20
		rep stosb
		mov WORD [PathDepth], bp
		popa
		ret


LOADMSG db "Loading File ", 0
PathDepth dw 0 ; max depth = 8
FilePath times (11 * 8) db 0

%endif
%endif
