; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0003
;                                HDD VBR (FAT16)
;                          05/02/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 16
org 0x7C00

DAP					equ 0x0500			; DAP Pointer
DATA_BUFFER			equ 0x2000			; Data Buffer Pointer (16KB)
FAT_BUFFER			equ 0x6000			; FAT Buffer (4KB)


VAR:
	.bootDrive		equ -127			; Boot Drive Number
	.FileSize		equ -20				; File Size
	.FAT_Start		equ -12				; FAT Start
	.DATA_Start		equ -8				; Data Start
	.Cluster		equ -4				; Current Cluster
BPB:
	.BPS			equ 0x0B			; Bytes Per Sector
	.SPC			equ 0x0D			; Sectors Per Cluster
	.Reserved		equ 0x0E			; Reserved
	.FATCount		equ 0x10			; Number of FATs
	.RootEnts		equ 0x11			; Number of Root Dir Entries
	.Sectors		equ 0x13			; Total Sectors
	.FatSize		equ 0x16			; Size of FAT
	.SPT			equ 0x18			; Sectors Per Track
	.HPC			equ 0x1A			; Heads Per Cylinder
	.Hidden			equ 0x1C			; Hidden Sectors

jmp start

times (3 - ($ - $$)) nop

times (59) db 0xBB						; BPB

start:
	mov bp, 0x7C00						; bp points to start of sector
	mov [bp + VAR.bootDrive], dl		; Save Boot Drive
	mov [bp + VAR.Cluster], eax	; Root DIR = Cluster 0
	mov ax, [bp + BPB.Reserved]			; ReservedSectors
	add eax, [bp + BPB.Hidden]			; HiddenSectors
	mov [bp + VAR.FAT_Start], eax		; FAT_Start
	mov [bp + VAR.DATA_Start], eax		; DATA_Start
	movzx eax, WORD [bp + BPB.FatSize]	; EAX = FATSIZE
	shl ax, 1							; Multiply FATSIZE by 2
	movzx edx, WORD [bp + BPB.RootEnts]	; DX = Root Entries
	shr dx, 4							; Root Entries /= 16
	add eax, edx						; FATSIZE += Root Entries
	add [bp + VAR.DATA_Start], eax		; DATA_Start = FATSIZE16

	mov si, BOOTPATH
	call FindFile
	jc ERROR
	mov di, 0x7E00
	call ReadFile
	jc ERROR

	jmp 0x7E00

ERROR:
	.halt:
		hlt
		jmp .halt

; DS:SI = 8.3 Formated Path Pointer
; CX = depth of path
FindFile:
	mov di, BUFFER						; DI = Data Buffer
	cmp DWORD [bp + VAR.Cluster], 0		; if(CurrentCluster)
	je .RootDir							; 	Jump to RootDir;
	call FAT_ReadCluster				; Read Cluster into Buffer
	shr cx, 4							; CX = Cluster Size, CX /= 16
	jmp .Search							; Search DIR
	.RootDir:
		mov eax, [bp + VAR.FAT_Start]	; LBA = FAT_Start
		movzx ebx,WORD[bp + BPB.FatSize]; EAX = FATSIZE16
		shl ebx, 1						; FATSIZE16 *= 2
		add eax, ebx					; LBA += FATSIZE16
		movzx cx, BYTE [bp + BPB.SPC]	; CX = SectorsPerCluster
		call ReadSectors				; Read Sectors
		shl cx, 4						; SectorsPerCluster *= 16
	.Search:
		.DirLoop:
			pusha						; Save Registers
			mov cx, 11					; CX = 11 (File Name Length 8.3)
			rep cmpsb					; Compare String DS:SI to ES:DI
			popa						; Restore Registers
			je .Found					; If equal we fount the file.
			add di, 0x20				; DIR Entry is 32 Bytes
			loop .DirLoop				; Loop
			jmp ERROR					; ERROR!
	.Found:
		mov ax, [di + 0x14]				; AX = High Cluster
		shl eax, 16						; EAX << 16
		mov ax, [di + 0x1A]				; AX = Low Cluster
		mov [bp + VAR.Cluster], eax		; CurrentCluster = EAX
		ret

ReadFile:
	pushad
	mov bp, 0x7C00
	popad
	stc
	ret
