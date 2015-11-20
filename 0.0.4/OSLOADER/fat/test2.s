[org 0x7C00]
[bits 16]

DAP equ 0x0800
BUFFER equ 0x1000

jmp start
times(3-($-$$)) nop
BPB:                          ; BIOS Parameter Block
	.OEM times 8 db 0         ; OEM identifier
	.SectorBytes dw 0         ; Bytes per logical sector
	.ClusterSectors db 0      ; Logical sectors per cluster
	.ReservedSectors dw 0     ; Reserved logical sectors
	.FATs db 0                ; Number of FATs
	.RootEntries dw 0         ; Root directory entries
	.Sectors dw 0             ; Total logical sectors
	.Media db 0               ; Media descriptor
	.FATSectors dw 0          ; Logical sectors per FAT
	.TrackSectors dw 0        ; Physical sectors per track
	.CylinderHeads dw 0       ; Number of heads
	.HiddenSectors dd 0       ; Hidden sectors
	.SectorsBig dd 0          ; Large total logical sectors
	.FAT32Sectors dd 0        ; Logical sectors per FAT
	.Unused dw 0              ; Mirroring flags etc.
	.Version dw 0             ; Version
	.RootCluster dd 0         ; Root directory cluster
	.FSInfoSel dw 0           ; Location of FS Information Sector
	.BackupSectors dd 0       ; Location of backup sector(s)
	.Reserved times 12 db 0   ; Reserved
	.DriveNumber db 0         ; Physical drive number
	.bpbFlags db 0            ; Flags etc.
	.ExtBootSignature db 0    ; Extended boot signature (0x29)
	.SerialNumber dd 0        ; Volume serial number
	.VolumeLabel times 11 db 0; Volume label
	.FileSystem times 8 db 0  ; File-system type

start:
	jmp 0:substart

substart:
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0xFFF8
	mov bp, 0x7C00
	mov BYTE [bp - 13], dl ; bootDrive
	movzx eax, WORD [bp + 0x0E] ; ReservedSectors
	add eax, DWORD [bp + 0x1C] ; HiddenSectors
	mov DWORD [bp - 12], eax ; FAT_Start
	mov DWORD [bp - 8], eax ; DATA_Start
xor edx, edx
	movzx eax, WORD [bp + 0x16] ; FATSIZE16
	test ax, ax
	jz .FAT32
	; Ok we are either FAT12 or FAT16...
;	shl eax, 1
shl ax, 1
;	movzx edx, WORD [bp + 0x11] ; Root Entries
mov dx, WORD [bp - 0x11]
	shr dx, 4
	add eax, edx
	add DWORD [bp - 8], eax ; DATA_Start
	xor eax, eax
	jmp .FATinitilized
	.FAT32:
		mov eax, DWORD [bp + 0x24] ; FATSIZE32
;		movzx edx, BYTE [bp + 0x10] ; FAT Count
mov dl, BYTE [bp + 0x10]
		mul edx
		add DWORD [bp - 8], eax
		mov eax, DWORD [bp + 0x2C] ; Root Cluster
	.FATinitilized:

		mov DWORD [bp - 4], eax ; CurrentCluster
		mov si, BOOTFILE
		.FindFile:
			mov di, BUFFER
			cmp DWORD [bp - 4], 0 ; CurrentCluster
			je .RootDir
			call FAT_ReadCluster
			xchg cx, ax
			shr cx, 4
			jmp .Search
			.RootDir:
				mov ebx, DWORD [bp - 12] ; FAT_Start
				movzx eax, WORD [bp + 0x16] ; FATSIZE16
				shl eax, 1
				add ebx, eax
				movzx cx, BYTE [bp + 0x0D] ; Sectors Per Cluster
				call ReadSectors
				shl cx, 4
			.Search:
				.DirLoop:
					pusha
					mov cx, 11
					rep cmpsb
					popa
					je .Found
					add di, 0x20
					loop .DirLoop
					jmp ERROR
			.Found:
				mov ax, WORD [di + 0x14] ; High Cluster
				shl eax, 16
				mov ax, WORD [di + 0x1A] ; Low Cluster
				mov DWORD [bp - 4], eax ; CurrentCluster
		mov di, WORD 0x7E00
		.FATLoadFile:
			call FAT_ReadCluster
			add di, ax
			cmp DWORD [bp - 4], 0 ; CurrentCluster
			jnz .FATLoadFile
		mov dl, BYTE [bp - 13] ; bootDrive
		jmp 0x7E00
ERROR:
	mov al, 0xFE
	out 0x64, al
	cli
	hlt

FAT_ReadCluster:
	mov eax, DWORD [bp - 4]   ; CurrentCluster
	sub eax, 2
	movzx ecx, BYTE [bp + 0x0D] ; Sectors Per Cluster
	mul ecx
	add eax, DWORD [bp - 8] ; DATA_Start
	xchg ebx, eax
	call ReadSectors          ; ReadSectors
	shl cx, 9                 ; Length = (SectorsPerCluster * 512)
	xchg ax, cx               ; Length reterns in AX

	pusha
	mov ebx, DWORD [bp - 12] ; FAT_Start
	mov edx, DWORD [bp - 4] ; CurrentCluster
	mov di, WORD BUFFER
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
	popa
	ret

ReadSectors:
	pusha                         ; Save Registers
	mov si, DAP
	mov WORD [si], 0x000C
	mov DWORD [si + 8], ebx       ; Setup Disk Address Packet, DAP LBA Low
	mov WORD [si + 6], 0
	mov WORD [si + 4], di         ; DAP Destination Buffer
	mov WORD [si + 2], cx         ; DAP Sector Count
	mov dl, BYTE [bp - 13]        ; bootDrive
	mov ax, 0x4200                ; Extended Read Function
	int 0x13                      ; Call BIOS
	jc ERROR                      ; if carry flag is set Error
	popa                          ; Restore Registers
	ret

times (499 - ($-$$)) db 0x90

BOOTFILE db "OSLOADERSYS"

dw 0xAA55