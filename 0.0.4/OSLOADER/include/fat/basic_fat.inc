%ifndef __BASIC_FAT_INC__INCLUDED__
%define __BASIC_FAT_INC__INCLUDED__

[bits 16]

; This driver has to work on OSLOADER too...
; Lets get the data directly from the memory address (not a label)
FAT16_SectorsPerCluster equ 0x7C0D ; BYTE
FAT16_ReserverdSectors equ 0x7C0E ; WORD
FAT16_NumberOfFATs equ 0x7C10 ; BYTE
FAT16_RootEntries equ 0x7C11 ; WORD
FAT16_SectorsPerFAT equ 0x7C16 ; WORD

FAT_FindFile_Root:
	pushad										; Save Registers
	movzx ebx, WORD [FAT16_SectorsPerFAT]		; Sector = SectorsPerFat
	shl bx, 1									; Sector *= 2
	add bx, WORD [FAT16_ReserverdSectors]		; Sector += ReserverdSectors
	add ebx, DWORD [PartitionOffset]			; Sector += PartitionOffset
	mov bp, WORD [FAT16_RootEntries]			; SectorCount = RootEntries
	shr bp, 4									; SectorCount /= 16
	.SectorLoop:
		mov edx, DWORD 0x1000					; Temp Buffer is 0x1000
		mov cx, 1								; We want One sector
		call ReadSectors						; ReadSectors
		mov ax, 16								; There are 16 entries per sector
		mov di, 0x1000							; Our buffer is 0x1000
		.DirLoop:
			push si								; Save FileName
			push di								; Save Buffer
			mov cx, 11							; 8.3 FileName is 11 charactors
			rep cmpsb							; Compare FileName
			pop di								; Restore Buffer
			pop si								; Restore FileName
			je .Found							; Did we find it?
			add di, 0x20						; Increment Buffer 32 Bytes
			dec ax								; Deincrement entry count
			jnz .DirLoop						; If we have more entries in this sector, loop
		inc ebx									; Sector ++
		dec bp									; SectorCount --
		jnz .SectorLoop							; if SectorCount, loop
	.NotFound:
		mov WORD [CurrentCluster], 0			; File Not Found, Set CurrentCluster to 0
		popad									; Restore Registers
		stc										; Set Carry
		ret										; Return
	.Found:
		mov ax, WORD [di + 0x001A]				; FirstCluster = WORD [Buffer + 0x1A]
		mov WORD [CurrentCluster], ax			; Save FirstCluster in CurrentCluster
		popad									; Restore Registers
		clc										; Make sure carry is clear
		ret										; Return
; di = location
; CurrentCluster = FirstCluster
FAT_LoadFile:
	pusha										; Save Registers
	mov DWORD [FileSize], 0						; Reset FileSize
	.ClusterLoop:
		call FAT_ReadCluster					; ReadCluster
		add DWORD [FileSize], eax				; Add Cluster Length to FileSize
		cmp WORD [CurrentCluster], 0			; Did something F*** up?
		je .Fail								; If so fail
		cmp WORD [CurrentCluster], 0xFFF0		; Did we read the last Cluster?
		jb .ClusterLoop							; If not loop.
		popa									; We are done, Restore Registers
		mov eax, DWORD [FileSize]				; FileSize returns in EAX
		clc										; Confirm Carry is clear
		ret										; Return
	.Fail:
		popa									; We failed, Restore Registers
		stc										; Set Carry
		ret										; Return

FAT_ReadCluster:
	push di										; Save DI and SI
	push si
	movzx ebx, WORD [FAT16_SectorsPerFAT]		; Sector = SectorsPerFat
	shl bx, 1									; Times 2
	add bx, WORD [FAT16_ReserverdSectors]		; Plus Reserved Sectors
	mov cx, WORD [FAT16_RootEntries]
	shr cx, 4
	add bx, cx									; Plus (RootEntries / 16)
	mov ax, WORD [CurrentCluster]
	sub ax, 2
	movzx cx, BYTE [FAT16_SectorsPerCluster]
	mul cx
	add bx, ax									; Plus ((CurrentCluster - 2) * SectorsPerCluster)
	add ebx, DWORD [PartitionOffset]			; Plus PartitionOffset
	mov edx, DWORD 0x1000						; Temp Buffer is 0x1000
	call ReadSectors							; ReadSectors
	mov si, 0x1000								; Temp Buffer location
	shl cx, 9									; Length = (SectorsPerCluster * 512)
	movzx eax, cx								; Length reterns in EAX
	rep movsb									; Copy to location requested in DI
	call FAT_NextCluster						; Find The Next Cluster
	pop si										; Restore SI and DI
	pop di
	ret											; Return

;----------------------------------------------------
;                    ReadSectors
;                EBX = Starting LBA
;                EDX = Destination
;                CX = Sector Count
;----------------------------------------------------
FAT_NextCluster:
	pushad										; Save Registers
	movzx ebx, WORD [FAT16_ReserverdSectors]	; Sector = ReserverdSectors
	add ebx, DWORD [PartitionOffset]			; Sector += PartitionOffset
	movzx ecx, WORD [CurrentCluster]
	movzx ax, cl								; Cluster's Offset = (CurrentCluster & 0xFF)
	shr cx, 8									; Cluster's Sector = (CurrentCluster >> 8)
	add ebx, ecx								; Sector += Cluster's Sector
	mov edx, DWORD 0x1000						; Temp Buffer is 0x1000
	mov cx, 1									; Read ONE sector
	call ReadSectors							; ReadSectors
	shl ax, 1									; Cluster's Offset *= 2
	add ax, 0x1000								; Add Temp Buffer Offset from 0x0000:0x0000
	mov bx, ax									; AX cannot be used in the next instruction so copy to BX
	mov cx, WORD [ds:bx]						; Get the next Cluster from the table in CX
	mov WORD [CurrentCluster], cx				; Store Next Cluster in CurrentCluster
	popad										; Restore Registers
	ret											; Return

CurrentCluster dw 0
FileSize dd 0

%endif
