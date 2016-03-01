[bits 16]
[org 0x7C00]

BUFFER				equ 0x1000
FATBUFFER			equ 0x3000

BP_OFF:
	.bootDrive		equ -127					; Boot Drive Number
	.bootInfo		equ -126					; BITFIELD
	.aHead			equ -124					; CHS Head
	.aSector		equ -123					; CHS Sector
	.aTrack			equ -122					; CHS Cylinder / Track
	.FileSize		equ -20						; File Size
	.FAT_Start		equ -12						; FAT Start
	.DATA_Start		equ -8						; Data Start
	.CurrentCluster	equ -4						; Current Cluster
	.bpb_BPS		equ 0x0B					; Bytes Per Sector
	.bpb_SPC		equ 0x0D					; Sectors Per Cluster
	.bpb_Reserved	equ 0x0E					; Reserved
	.bpb_FATCount	equ 0x10					; Number of FATs
	.bpb_RootEnts	equ 0x11					; Number of Root Dir Entries
	.bpb_Sectors	equ 0x13					; Total Sectors
	.bpb_FatSize	equ 0x16					; Size of FAT
	.bpb_SPT		equ 0x18					; Sectors Per Track
	.bpb_HPC		equ 0x1A					; Heads Per Cylinder
	.bpb_Hidden		equ 0x1C					; Hidden Sectors

jmp start										; Jump past BPB area

times (3 - ($-$$)) nop

times (59) db 0xBB

start:
	cli
	xor eax, eax								; Fix Segments
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0xFFF8
	jmp 0:FixedSegments

FixedSegments:
	sti
	mov bp, 0x7C00								; bp points to start of sector
	mov [bp + BP_OFF.bootDrive], dl				; Save Boot Drive
	mov [bp + BP_OFF.CurrentCluster], eax		; Root DIR = Cluster 0
	mov BYTE [bp + BP_OFF.bootInfo], 0x41
	mov ax, [bp + BP_OFF.bpb_Reserved]			; ReservedSectors
	add eax, [bp + BP_OFF.bpb_Hidden]			; HiddenSectors
	mov [bp + BP_OFF.FAT_Start], eax			; FAT_Start
	mov [bp + BP_OFF.DATA_Start], eax			; DATA_Start
	movzx eax, WORD [bp + BP_OFF.bpb_FatSize]	; EAX = FATSIZE
	; Ok we are either FAT12 or FAT16...
	shl ax, 1									; Multiply FATSIZE by 2
	movzx edx, WORD [bp + BP_OFF.bpb_RootEnts]	; DX = Root Entries
	shr dx, 4									; Root Entries /= 16
	add eax, edx								; FATSIZE += Root Entries
	add [bp + BP_OFF.DATA_Start], eax			; DATA_Start = FATSIZE16

	mov si, BOOTDIR								; SI = Boot Directory
	call FindFile								; Find Boot Directory
	jc ERROR
	mov si, BOOTFILE							; SI = Boot File
	call FindFile								; Find Boot File
	jc ERROR

	mov di, 0x7E00								; Destination Offset is 0x7E00
	.FATLoadFile:
		call FAT_ReadCluster					; Read Cluster
		add di, cx								; Destination += Cluster Size
		cmp DWORD[bp + BP_OFF.CurrentCluster], 0; if(CurrentCluster)
		jnz .FATLoadFile						; 	loop;
	mov dl, [bp + BP_OFF.bootDrive]				; DL = bootDrive
	jmp 0x7E00									; Jump to BOOTFILE
	

ERROR:
	mov si, ERROR_MSG
	call puts
	.halt:
		hlt
		jmp .halt

ReadSectors:
	pusha
	mov bx, di
	.Main:
		mov di, 5
	.SectorLoop:
		pusha
		.LBAtoCHS:
			xor dx, dx							; prepare dx:ax for operation
			div WORD [bp + BP_OFF.bpb_SPT]		; calculate
			inc dl								; adjust for sector 0
			mov [bp + BP_OFF.aSector], dl
			xor dx, dx							; prepare dx:ax for operation
			div WORD [bp + BP_OFF.bpb_HPC]		; calculate
			mov [bp + BP_OFF.aHead], dl
			mov [bp + BP_OFF.aTrack], al
		.ReadSector:
			mov ax, 0x0201						; Function 2, count 1
			mov cx, [bp + BP_OFF.aSector]		; ch = aTrack cl = aSector
			mov dx, [bp + BP_OFF.bootDrive]		; dh = aHead dl = bootDrive
			int 0x13							; INT 0x13 (BIOS Disk)
			jnc .ReadFine						; if CF = 0 We read the sector
			xor ax, ax							; Else Reset Disk
			int 0x13
		popa
		dec di									; Decrement error counter
		jnz .SectorLoop							; Retry if error counter > 0
		jmp ERROR								; Else ABORT
	.ReadFine:
		popa
		add bx, [bp + BP_OFF.bpb_BPS]			; Adjust Buffer Offset by BPS
		inc ax									; Increment LBA
		loop .Main								; Loop through all sectors (cx)
	popa
	ret

FindFile:
	mov di, BUFFER								; DI = Data Buffer
	cmp DWORD [bp + BP_OFF.CurrentCluster], 0	; if(CurrentCluster)
	je .RootDir									; 	Jump to RootDir;
	call FAT_ReadCluster						; Read Cluster into Buffer
	shr cx, 4									; CX = Cluster Size, CX /= 16
	jmp .Search									; Search DIR
	.RootDir:
		mov eax, [bp + BP_OFF.FAT_Start]		; LBA = FAT_Start
		movzx ebx,WORD [bp + BP_OFF.bpb_FatSize]; EAX = FATSIZE16
		shl ebx, 1								; FATSIZE16 *= 2
		add eax, ebx							; LBA += FATSIZE16
		movzx cx, BYTE [bp + BP_OFF.bpb_SPC]	; CX = SectorsPerCluster
		call ReadSectors						; Read Sectors
		shl cx, 4								; SectorsPerCluster *= 16
	.Search:
		.DirLoop:
			pusha								; Save Registers
			mov cx, 11							; CX = 11 (File Name Length 8.3)
			rep cmpsb							; Compare String DS:SI to ES:DI
			popa								; Restore Registers
			je .Found							; If equal we fount the file.
			add di, 0x20						; DIR Entry is 32 Bytes
			loop .DirLoop						; Loop
			jmp ERROR							; ERROR!
	.Found:
		mov ax, [di + 0x14]						; AX = High Cluster
		shl eax, 16								; EAX << 16
		mov ax, [di + 0x1A]						; AX = Low Cluster
		mov [bp + BP_OFF.CurrentCluster], eax	; CurrentCluster = EAX
		ret

FAT_ReadCluster:
	mov eax, [bp + BP_OFF.CurrentCluster]		; EAX = CurrentCluster
	sub eax, 2									; CurrentCluster -= 2
	movzx ecx, BYTE [bp + BP_OFF.bpb_SPC]		; ECX = SectorsPerCluster
	mul ecx										; CurrentCluster *= SectorsPerCluster
	add eax, [bp + BP_OFF.DATA_Start]			; CurrentCluster += DATA_Start
	;xchg ebx, eax								; EBX = LBA
	call ReadSectors							; ReadSectors
	
	xchg ax, cx
	mov dx, [bp + BP_OFF.bpb_BPS]				; cx = Sectors * BPS
	mul dx
	xchg ax, cx

	pusha										; Save Registers
	mov ebx, [bp + BP_OFF.FAT_Start]			; EBX = FAT_Start
	mov edx, [bp + BP_OFF.CurrentCluster]		; EDX = CurrentCluster
	mov di, FATBUFFER							; DI = FAT Buffer
	mov cx, 2									; Count = 2 (Sectors)

	mov ax, [bp + BP_OFF.bpb_RootEnts]			; AX = Root Entries
	mov ax, [bp + BP_OFF.bpb_Sectors]			; AX = total secotrs 16
	mov ax, dx									; AX = CurrentCluster
	shr ax, 1									; AX /= 2
	add ax, dx									; AX += CurrentCluster
	mov dx, ax									; DX = AX
	shr dx, 8									; DX >> 8
	add ebx, edx								; LBA (EBX) += EDX
	xchg eax, ebx
	call ReadSectors							; ReadSectors
	movzx bx, al								; ax cannot be used... bx can...
	mov ax, [di + bx]							; Get Cluster (Kind Of)
	test bl, 1									; Test for Even / Odd Cluster
	jz .FAT12EVEN								; if 0 then EVEN
	shr ax, 4									; Use High 12-bits for ODD
	.FAT12EVEN:
		and ax, 0x0FFF							; Use Low 12-bits for EVEN
	cmp ax, 0x0FF0 								; Check for End of Chain
	jbe .continue								; Continue
	.EOF:
		xor eax, eax							; Clear EAX (New Cluster)
	.continue:
		mov [bp + BP_OFF.CurrentCluster], eax	; Save New Cluster
	popa										; Restore Registers
	ret											; Return

puts:
	pusha										; Save Registers
	.StrLoop:
		lodsb									; mov al, BYTE [ds:si]; inc si
		test al, al								; test al for 0
		jz .Done								; Check for Terminator (0)
		mov ah, 0Eh								; Print Char To Screen Function
		int 10h									; Call BIOS
		jmp .StrLoop							; Loop
	.Done:
		popa									; Restore Registers
		ret										; Return

ERROR_MSG db "ERROR: Failed to boot!", 0

;times (0x01E8 - ($ - $$)) nop

BOOTDIR  db "BOOT       "						; Boot Folder
BOOTFILE db "SYSINIT BIN"						; Boot File

dw 0xAA55										; Boot Signiture
