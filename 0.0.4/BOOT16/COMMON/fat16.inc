;----------------------------------------------------
;                   fat16.inc
;                   2014-08-14
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __FAT16_INC__INCLUDED__
%define __FAT16_INC__INCLUDED__

[bits 16]

NextCluster:
	pushad
	mov ebx, DWORD [PTOffset]
	add bx, WORD [bpbReservedSectors]
	mov cx, WORD [CurrentCluster]
	shr cx, 8 ; Multipy by 2 then divide by 512 (or divide by 256)
	mov edx, DWORD 0x7E00
	mov cx, 1
	call ReadSectors
	mov bx, WORD [CurrentCluster]
	shl bx, 1
	add bx, 0x200
	push es
	mov ax, 0x07C0
	mov es, ax
	mov bx, WORD [es:bx]
	pop es
	mov WORD [CurrentCluster], bx
	popad
	ret

FindFileInRoot:
	pushad
	mov EBX, DWORD [PTOffset] ; Start with Partition Start LBA
	mov ax, WORD [bpbSectorsPerFAT]
	shl ax, 1
	add bx, ax ; Add Sectors Per Fat * 2
	add bx, WORD [bpbReservedSectors]
	mov cx, WORD [bpbRootEntries]
	shr cx, 4
push es
mov ax, 0x07C0
mov es, ax
	.SectorLoop:
		push cx
		mov EDX, DWORD 0x8000
		mov cx, 1
		call ReadSectors
		mov cx, 0x10
		mov di, 0x400
		.FileDirLoop:
			push cx
			push si
			push di
			mov cx, 11
			rep cmpsb
			pop di
			pop si
			pop cx
			je .FileDirFound
			add di, 32
			loop .FileDirLoop
			pop cx
		inc bx
		loop .SectorLoop
pop es
		popad
		stc
		ret
	.FileDirFound:
		pop cx
		mov dx, WORD [es:di + 0x001A]  ; start cluster		
pop es
		mov WORD [CurrentCluster], dx
		popad
		ret

LoadFile:
	mov ECX, DWORD 0
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

	call NextCluster
	mov cx, WORD [CurrentCluster]
	db 0x66, 0x67
		cmp ecx, 0xFFF0 ; Have we made it to the last cluster
	jge .done ; Yep
	xor bx, bx
	mov bl, BYTE [bpbSectorsPerCluster]
	shl bx, 9
	add DX, BX ; Move destination pointer by Sectors Per Cluster * 512
	jmp LoadFile
	.done:
	ret

CurrentCluster dw 0

%endif	;__FAT16_INC__INCLUDED__