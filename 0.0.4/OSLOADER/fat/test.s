[org 0x7C00]
[bits 16]

BPB_SECTSPERCLUST equ 0x7C0D
BPB_RESERVSECTS equ 0x7C0E
BPB_FATCOUNT equ 0x7C10
BPB_ROOTENTS equ 0x7C11
BPB_TOTALSECTS16 equ 0x7C13
BPB_FATSIZE16 equ 0x7C16
BPB_HIDDENSECTS equ 0x7C1C
BPB_FATSIZE32 equ 0x7C24
BPB_ROOTCLUST equ 0x7C2C

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
	.Flags db 0               ; Flags etc.
	.ExtBootSignature db 0    ; Extended boot signature (0x29)
	.SerialNumber dd 0        ; Volume serial number
	.VolumeLabel times 11 db 0; Volume label
	.FileSystem times 8 db 0  ; File-system type

start:
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0xFFF8
	mov BYTE [bootDrive], dl
	call FAT_init
	mov si, BOOTFILE
	call FAT_FindFile
	mov edi, DWORD 0x7E00
	call FAT_ReadFile
	mov ax, di
	jmp 0:ax
ERROR:
	cli
	hlt

FAT_ReadFile:
	push di
	xor cx, cx
	.loop:
		call FAT_ReadCluster
		add di, ax
		add cx, ax
		cmp DWORD [CurrentCluster], 0
		jnz .loop
	pop di
	ret

FAT_ReadCluster:
	mov eax, DWORD [CurrentCluster]
	sub eax, 2
	movzx edx, BYTE [BPB_SECTSPERCLUST]
	mul edx
	add eax, DWORD [DATA_Start]
	xchg ebx, eax
	call ReadSectors          ; ReadSectors
	shl cx, 9                 ; Length = (SectorsPerCluster * 512)
	xchg ax, cx                ; Length reterns in AX
	call FAT_NextCluster      ; Find The Next Cluster
	ret

FAT_NextCluster:
	pushad
	mov ebx, DWORD [FAT_Start]
	mov edx, DWORD [CurrentCluster]
	mov edi, DWORD 0x1000
	mov cx, 2
	movzx eax, WORD [BPB_TOTALSECTS16]
	test ax, ax
	jz .FAT32
	movzx edx, BYTE [BPB_SECTSPERCLUST]
	div dx
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
		jnz .FAT12ODD
		and ax, 0x0FFF
		jmp .continue
		.FAT12ODD:
			shr ax, 4
			jmp .continue
	.FAT16:
		movzx ax, dl
		shr dx, 8
		add ebx, edx
		call ReadSectors
		shl ax, 1
		xchg bx, ax
		mov ax, WORD [di + bx]
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
	.continue:
		mov DWORD [CurrentCluster], eax
		popad
		ret

FAT_FindFile:
	pusha
	mov edi, DWORD 0x1000
	cmp DWORD [CurrentCluster], 0
	je .ROOTDIR
	.CLUSTERDIR:
;		cmp DWORD [CurrentCluster], 0
;		je ERROR
		call FAT_ReadFile
		shl cx, 4
		jmp .Search
	.ROOTDIR:
		mov ebx, DWORD [FAT_Start]
		movzx eax, WORD [BPB_FATSIZE16]
		shl eax, 2
		add ebx, eax
		mov cx, WORD [BPB_ROOTENTS]
		shr cx, 4
		call ReadSectors
		shl cx, 8
	.Search:
		pusha
		mov cx, 11
		rep cmpsb
		popa
		je .Found
		add di, 0x20
		loop .Search
		jmp ERROR
		.Found:
			mov ax, WORD [di + 0x14]
			shl eax, 16
			mov ax, WORD [di + 0x1A]
			mov DWORD [CurrentCluster], eax
			popa
			ret

FAT_init:
	movzx eax, WORD [BPB_RESERVSECTS]
	add eax, DWORD [BPB_HIDDENSECTS]
	mov DWORD [FAT_Start], eax
	mov DWORD [DATA_Start], eax

	movzx eax, WORD [BPB_FATSIZE16]
	test ax, ax
	jz .FAT32
	; Ok we are either FAT12 or FAT16...
	shl eax, 1
	movzx edx, WORD [BPB_ROOTENTS]
	shr dx, 4
	add eax, edx
	add DWORD [DATA_Start], eax
	ret
	.FAT32:
		mov eax, DWORD [BPB_FATSIZE32]
		movzx edx, BYTE [BPB_FATCOUNT]
		mul edx
		add DWORD [DATA_Start], eax
		mov eax, DWORD [BPB_ROOTCLUST]
		mov DWORD [CurrentCluster], eax
		ret

ReadSectors:
	pusha                         ; Save Registers
	mov DWORD [DAP.LBALow], ebx   ; Setup Disk Address Packet, DAP LBA Low
	mov DWORD [DAP.dest], edi     ; DAP Destination Buffer
	mov WORD [DAP.count], cx      ; DAP Sector Count
	mov dl, BYTE [bootDrive]      ; Set Boot Drive
	mov si, DAP                   ; Load DAP in SI
	mov ax, 0x4200                ; Extended Read Function
	int 0x13                      ; Call BIOS
	jc ERROR                      ; if carry flag is set Error
	popa                          ; Restore Registers
	ret

CurrentCluster dd 0
FAT_Start dd 0
DATA_Start dd 0
bootDrive db 0

DAP:                          ; Disk Address Packet
	db 0x10                   ; Size of DAP (0x10 or 16 bytes)
	db 0                      ; Unused
	.count: dw 0              ; Sector Count
	.dest: dd 0               ; Destination Buffer
	.LBALow: dd 0             ; Low 32-bit LBA
	.LBAHigh: dd 0            ; High 32-bit LBA (I Don't Support!)

BOOTFILE db "OSLOADERSYS"

times (510 - ($-$$)) db 0x90
dw 0xAA55
