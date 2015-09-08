;----------------------------------------------------
;                  fat16ext.inc
;                   2014-08-17
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __FAT16EXT_INC__INCLUDED__
%define __FAT16EXT_INC__INCLUDED__

[bits 16]

; ReadCluster
; EDX = Destination (16 Bit OFFSET:SEGMENT)
; CurrentCluster read to EDX
ReadCluster:
	pushad
	xor cx, cx
	mov cl, BYTE [bpbSectorsPerCluster] ; Count is Sectors Per Cluster
	mov EBX, DWORD [PTOffset] ; Start with Partition Start LBA

	mov ax, WORD [bpbSectorsPerFAT]
	shl ax, 1
	add bx, ax ; Add Sectors Per Fat * 2

	add bx, WORD [bpbReservedSectors]

	mov ax, WORD [bpbRootEntries]
	shr ax, 4
	add bx, ax ; Add Root Entries / 16

	mov ax, WORD [CurrentCluster]
	sub ax, 2
	push dx
	mul cx
	pop dx
	add bx, ax ; add (CurrentCluster - 2) * Sectors Per Cluster

	call ReadSectors
	popad
	ret

; Find File
; AX = Current Dir Cluster (0 for root)
; SI = File Name
; Output WORD CurrentCluster
FindFile:
	test ax, ax
	jz FindFileInRoot
	pushad
	
	.ClusterLoop:
		mov WORD [CurrentCluster], ax
		mov edx, DWORD 0xCE00
		call ReadCluster
		call NextCluster
		mov di, 0x1000
		mov cx, WORD [bpbSectorsPerCluster]
		shl cx, 4 ; 16 Entries Per Sector
		.Loop:
			push cx
			push si
			push di
			mov cx, 11
			rep cmpsb
			pop di
			pop si
			pop cx
			je .Found
			add di, 32
			loop .Loop
		mov edx, DWORD 0
		mov dx, WORD [CurrentCluster]
		cmp edx, DWORD 0xFFF0
		jl .ClusterLoop
	popad
	stc
	ret

	.Found:
		mov dx, WORD [es:di + 0x001A];  ; start cluster
		mov WORD [CurrentCluster], dx
		popad
		ret

; LoadFileLong
; EDX = Destination
; CurrentCluster (start Cluster)
LoadFileLong:
	push bx
	mov bx, 0
	.looping:
	inc bx
	pushad
	db 0x66, 0x67
		mov EDI, EDX
	mov ESI, DWORD 0x2000
	mov EDX, DWORD 0x2000
	call ReadCluster
	db 0x66, 0x67
		xor ecx, ecx
	mov cl, BYTE [bpbSectorsPerCluster]
	shl cx, 7
	call memcpy
	call NextCluster
	mov ecx, dword 0
	mov cx, WORD [CurrentCluster]
	db 0x66, 0x67
		cmp ecx, 0xFFF0
	jge .FileLoaded

	popad

	db 0x66, 0x67
		push eax
	db 0x66, 0x67
		xor eax, eax
	mov al, BYTE [bpbSectorsPerCluster]
	db 0x66, 0x67
		shl eax, 9
	db 0x66, 0x67
		add edx, eax
	db 0x66, 0x67
		pop eax
	jmp .looping

	.FileLoaded:
		popad
		push ax
		push dx
		mov ax, bx
		mul BYTE [bpbSectorsPerCluster]
		mov WORD [CurFileSectorCount], ax
		pop dx
		pop ax
		pop bx
		ret

CurFileSectorCount dw 0

%endif ; __FAT16EXT_INC__INCLUDED__