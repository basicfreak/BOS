[bits 16]
[org 0x7C00]

DAP					equ 0x0500
BUFFER				equ 0x1000
FATBUFFER			equ 0x3000

BP_OFF:
	.bootDrive		equ -127
	.FileSize		equ -20
	.FAT_Start		equ -12
	.DATA_Start		equ -8
	.CurrentCluster	equ -4
	.bpb_BPS		equ 0x0B
	.bpb_SPC		equ 0x0D
	.bpb_Reserved	equ 0x0E
	.bpb_FATCount	equ 0x10
	.bpb_RootEnts	equ 0x11
	.bpb_16_Sectors	equ 0x13
	.bpb_16_FatSize	equ 0x16
	.bpb_Hidden		equ 0x1C
	.bpb_32_FatSize	equ 0x24
	.bpb_RootClust	equ 0x2C

jmp start										; Jump past BPB area

times (3 - ($-$$)) nop

times (79) db 0xBB

start:
	xor eax, eax								; 0 EAX
	mov bp, 0x7C00								; bp points to start of sector
	mov [bp + BP_OFF.bootDrive], dl				; bootDrive
	mov ax, [bp + BP_OFF.bpb_Reserved]			; ReservedSectors
	add eax, [bp + BP_OFF.bpb_Hidden]			; HiddenSectors
	mov [bp + BP_OFF.FAT_Start], eax			; FAT_Start
	mov [bp + BP_OFF.DATA_Start], eax			; DATA_Start
	xor edx, edx								; 0 EDX
	movzx eax, WORD [bp + BP_OFF.bpb_16_FatSize]; EAX = FATSIZE16
	test ax, ax									; Does this value exist
	jz .FAT32									; No, This is FAT32
	; Ok we are either FAT12 or FAT16...
	shl ax, 1									; Multiply FATSIZE16 by 2
	mov dx, [bp + BP_OFF.bpb_RootEnts]			; DX = Root Entries
	shr dx, 4									; Root Entries /= 16
	add eax, edx								; FATSIZE16 += Root Entries
	add [bp + BP_OFF.DATA_Start], eax			; DATA_Start = FATSIZE16
	xor eax, eax								; No Root Cluster, 0 EAX
	jmp .FATinitilized							; We are finished here
	.FAT32:
		mov eax, [bp + BP_OFF.bpb_32_FatSize]	; EAX = FATSIZE32
		mov dl, [bp + BP_OFF.bpb_FATCount]		; EDX = FAT Count
		mul edx									; FATSIZE32 *= FAT Count
		add [bp + BP_OFF.DATA_Start], eax		; DATA_Start += FATSIZE32
		mov eax, [bp + BP_OFF.bpb_RootClust]	; EAX = Root Cluster
	.FATinitilized:
		mov [bp + BP_OFF.CurrentCluster], eax	; CurrentCluster = Root Cluster

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
	mov ax, 0x0E45								; Put an E on the Screen
	int 0x10									; With BIOS INT 10h 0Eh
	.halt:
		hlt										; Hault the CPU until interrupt
		jmp .halt								; continue to halt CPU

FindFile:
	mov di, BUFFER								; DI = Data Buffer
	cmp DWORD [bp + BP_OFF.CurrentCluster], 0	; if(CurrentCluster)
	je .RootDir									; 	Jump to RootDir;
	call FAT_ReadCluster						; Read Cluster into Buffer
	shr cx, 4									; CX = Cluster Size, CX /= 16
	jmp .Search									; Search DIR
	.RootDir:
		mov ebx, [bp + BP_OFF.FAT_Start]		; LBA = FAT_Start
		movzx eax,WORD[bp+BP_OFF.bpb_16_FatSize]; EAX = FATSIZE16
		shl eax, 1								; FATSIZE16 *= 2
		add ebx, eax							; LBA += FATSIZE16
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
	xchg ebx, eax								; EBX = LBA
	call ReadSectors							; ReadSectors
	shl cx, 9									; Length = (SectorsPerCluster * 512)

	pusha										; Save Registers
	mov ebx, [bp + BP_OFF.FAT_Start]			; EBX = FAT_Start
	mov edx, [bp + BP_OFF.CurrentCluster]		; EDX = CurrentCluster
	mov di, FATBUFFER							; DI = FAT Buffer
	mov cx, 2									; Count = 2 (Sectors)

	mov ax, [bp + BP_OFF.bpb_RootEnts]			; AX = Root Entries
	test ax, ax									; if (!Root Entries)
	jz .FAT32									; 	We are FAT32;
	mov ax, [bp + BP_OFF.bpb_16_Sectors]		; AX = total secotrs 16
	test ax, ax									; if (!Total Sectors 16)
	jz .FAT16									; 	We are FAT16;
	push dx										; Save DX
	movzx dx, BYTE [bp + BP_OFF.bpb_SPC]		; DX = SectorsPerCluster
	div dx										; Total Sectors 16 /= SectorsPerCluster
	pop dx										; Restore DX
	cmp ax, 0x0FF6								; if(AX > 0x0FF6)
	ja .FAT16									;	We are FAT16;

	.FAT12:
		mov ax, dx								; AX = CurrentCluster
		shr ax, 1								; AX /= 2
		add ax, dx								; AX += CurrentCluster
		mov dx, ax								; DX = AX
		shr dx, 8								; DX >> 8
		add ebx, edx							; LBA (EBX) += EDX
		call ReadSectors						; ReadSectors
		movzx bx, al							; ax cannot be used... bx can...
		mov ax, [di + bx]						; Get Cluster (Kind Of)
		test bl, 1								; Test for Even / Odd Cluster
		jz .FAT12EVEN							; if 0 then EVEN
		shr ax, 4								; Use High 12-bits for ODD
		.FAT12EVEN:
			and ax, 0x0FFF						; Use Low 12-bits for EVEN
		cmp ax, 0x0FF0 							; Check for End of Chain
		jae .EOF								; We fount the End Of File
		jmp .continue							; Continue
	.FAT16:
		movzx ax, dl							; AX = Low 8-Bits of CurrentCluster
		shr dx, 8								; DX >> 8
		add ebx, edx							; LBA (EBX) += EDX
		call ReadSectors						; ReadSectors
		shl ax, 1								; 2 Bytes Per Cluster
		xchg bx, ax								; ax cannot be used... bx can...
		mov ax, [di + bx]						; Get Cluster
		cmp ax, 0xFFF0 							; Check for End of Chain
		jae .EOF								; We found the End Of File
		jmp .continue							; Continue
	.FAT32:
		mov al, dl								; AL = Low 8-Bits of CurrentCluster
		and ax, 0xF								; AND AX 0xF (Only keep low 4-bits)
		shr edx, 4								; EDX >> 4
		add ebx, edx							; LBA (EBX) += EDX
		call ReadSectors						; ReadSectors
		shl ax, 2								; 4 Bytes Per Cluster
		xchg bx, ax								; ax cannot be used... bx can...
		mov eax, [di + bx]						; Get Cluster
		cmp eax, 0x0FFFFFF0						; Check for End of Chain
		jb .continue							; Continue
	.EOF:
		xor eax, eax							; Clear EAX (New Cluster)
	.continue:
		mov [bp + BP_OFF.CurrentCluster], eax	; Save New Cluster
	popa										; Restore Registers
	ret											; Return

ReadSectors:
	pusha										; Save Registers
	mov si, DAP									; Setup Disk Address Packet
	mov WORD [si], 0x000C						; DAP Size, and DAP Unused
	mov [si + 8], ebx							; DAP LBA Low
	mov [si + 6], es							; Destination in segment 0
	mov [si + 4], di							; DAP Destination Buffer
	mov [si + 2], cx							; DAP Sector Count
	mov dl, [bp + BP_OFF.bootDrive]				; bootDrive
	mov ax, 0x4200								; Extended Read Function
	int 0x13									; Call BIOS
	jc ERROR									; if carry flag is set Error
	popa										; Restore Registers
	ret											; Return

times (0x01E8 - ($ - $$)) nop

BOOTDIR  db "BOOT       "						; Boot Folder
BOOTFILE db "FAT-HDD BIN"						; Boot File

dw 0xAA55										; Boot Signiture
