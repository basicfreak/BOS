%ifndef __TFTP_INC__INCLUDED__
%define __TFTP_INC__INCLUDED__
%ifndef __FAT_INC__INCLUDED__
%define __FAT_INC__INCLUDED__

BUFFER equ 0x1000
FATBUFFER equ 0x7000

; ds:si = file path
; edx = destination
ReadFile:
	pushad
	push edx

	push si
	mov si, LOADMSG
	call puts
	pop si
	call puts

	mov bp, 0x7C00
	xor eax, eax
	mov DWORD [FileSize], 0
	cmp WORD [bp + 0x11], 0
	jnz .SectorRoot
	mov eax, DWORD [bp + 0x2C] ; Root Cluster
	.SectorRoot:
		mov DWORD [bp - 4], eax		
	call ConvertPath							; Convert the path to something useful.
	mov si, FilePath							; Move ConvertedPath location to si
	mov cx, WORD [PathDepth]
	.DirectoryLoop:
		call FAT_SearchDIR
		jc ERROR
		add si, 11
		dec cx
		jnz .DirectoryLoop
	.FileFound:
		push si
		mov si, WAITMSG
		call puts
		pop si

		mov di, BUFFER
		call FAT_ReadCluster
		movzx ecx, cx
		add DWORD [FileSize], ecx
		mov esi, BUFFER
		pop edi
		call memcpy
		push edi
		cmp DWORD [bp - 4], 0
		jnz .FileFound
	pop edx
	mov si, DONEMSG
	call puts
	popad
	mov eax, DWORD [FileSize]					; FileSize returns in eax
	ret

FAT_SearchDIR:
	pushad
	mov bp, 0x7C00
	mov di, BUFFER
	cmp DWORD [bp - 4], 0 ; CurrentCluster
	je .RootDir
	.ClusterDir:
		call FAT_ReadCluster
		shr cx, 4
		call .Search
		jnc .Finished
		cmp DWORD [bp - 4], 0
		jnz .ClusterDir
		popad
		stc
		ret
	.RootDir:
		mov ebx, DWORD [bp - 12] ; FAT_Start
		movzx eax, WORD [bp + 0x16] ; FATSIZE16
		shl eax, 1
		add ebx, eax
		movzx cx, BYTE [bp + 0x0D] ; Sectors Per Cluster
		call ReadSectors
		shl cx, 4
		call .Search
		jnc .Finished
		popad
		stc
		ret
	.Search:
		pusha
		mov cx, 11
		rep cmpsb
		popa
		je .Found
		add di, 0x20
		loop .Search
		stc
		ret
	.Found:
		mov ax, WORD [di + 0x14] ; High Cluster
		shl eax, 16
		mov ax, WORD [di + 0x1A] ; Low Cluster
		mov DWORD [bp - 4], eax ; CurrentCluster
		clc
		ret
	.Finished:
		popad
		ret

FAT_ReadCluster:
	push eax
	push ebx
	push edx
	push bp
	mov bp, 0x7C00
	mov eax, DWORD [bp - 4]   ; CurrentCluster
	sub eax, 2
	movzx ecx, BYTE [bp + 0x0D] ; Sectors Per Cluster
	mul ecx
	add eax, DWORD [bp - 8] ; DATA_Start
	xchg ebx, eax
	call ReadSectors          ; ReadSectors
	shl cx, 9                 ; Length = (SectorsPerCluster * 512)
	pop bp
	pop edx
	pop ebx
	pop eax

FAT_NextCluster:
	pushad
	mov bp, 0x7C00
	mov ebx, DWORD [bp - 12] ; FAT_Start
	mov edx, DWORD [bp - 4] ; CurrentCluster
	mov di, WORD FATBUFFER
	mov cx, 2

	mov ax, WORD [bp + 0x11] ; Root Entries
	test ax, ax
	jz .FAT32
	mov ax, WORD [bp + 0x13] ; total secotrs 16
	test ax, ax
	jz .FAT16
	push dx
	movzx dx, BYTE [bp + 0x0D] ; sectors per cluster
	div dx
	pop dx
	cmp ax, 0x0FF6
	ja .FAT16

	.FAT12:
		mov ax, dx
		shr ax, 1
		add ax, dx
		mov dx, ax
		shr dx, 8
		add ebx, edx
		call ReadSectors
		movzx bx, al
		mov ax, WORD [di + bx]
		test bl, 1
		jz .FAT12EVEN
		shr ax, 4
		.FAT12EVEN:
			and ax, 0x0FFF
		cmp ax, 0x0FF0
		jae .EOF
		jmp .continue
	.FAT16:
		movzx ax, dl
		shr dx, 8
		add ebx, edx
		call ReadSectors
		shl ax, 1
		xchg bx, ax
		mov ax, WORD [di + bx]
		cmp ax, 0xFFF0
		jae .EOF
		jmp .continue
	.FAT32:
		mov al, dl
		and ax, 0xF
		shr edx, 4
		add ebx, edx
		call ReadSectors
		shl ax, 2
		xchg bx, ax
		mov eax, DWORD [di + bx]
		cmp eax, 0x0FFFFFF0
		jb .continue
	.EOF:
		xor eax, eax
	.continue:
		mov DWORD [bp - 4], eax
	popad
	ret

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
FileSize dd 0

%endif
%endif