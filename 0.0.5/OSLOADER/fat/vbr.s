[bits 16]
[org 0x7C00]

DAP equ 0x0800
BUFFER equ 0x1000
FATBUFFER equ 0x7000

jmp start											; Jump past BPB area
times(3-($-$$)) nop									; Pad For BPB area

times (62 - ($-$$)) db 0x16							; FAT12/16 BPB
times (90 - ($-$$)) db 0							; FAT32 BPB

start:
	cli												; Stop Interrupts
	jmp 0:substart									; Fix (0) Code Segment

substart:
	xor eax, eax									; 0 EAX
	mov ds, ax										; 0 Data Segment
	mov es, ax										; 0 Extra Segment
	mov ss, ax										; 0 Stack Segment
	mov sp, 0xFFF8									; Stack Pointer = 0xFFF8
	mov bp, 0x7C00									; Base Pointer (BPB Pointer)
	sti												; Start Interrupts
	mov BYTE [bp - 13], dl							; bootDrive
	mov ax, WORD [bp + 0x0E]						; ReservedSectors
	add eax, DWORD [bp + 0x1C]						; HiddenSectors
	mov DWORD [bp - 12], eax						; FAT_Start
	mov DWORD [bp - 8], eax							; DATA_Start
	xor edx, edx									; 0 EDX
	movzx eax, WORD [bp + 0x16]						; EAX = FATSIZE16
	test ax, ax										; Does this value exist
	jz .FAT32										; No, This is FAT32
	; Ok we are either FAT12 or FAT16...
	shl ax, 1										; Multiply FATSIZE16 by 2
	mov dx, WORD [bp + 0x11]						; DX = Root Entries
	shr dx, 4										; Root Entries /= 16
	add eax, edx									; FATSIZE16 += Root Entries
	add DWORD [bp - 8], eax							; DATA_Start = FATSIZE16
	xor eax, eax									; No Root Cluster, 0 EAX
	jmp .FATinitilized								; We are finished here
	.FAT32:
		mov eax, DWORD [bp + 0x24]					; EAX = FATSIZE32
		mov dl, BYTE [bp + 0x10]					; EDX = FAT Count
		mul edx										; FATSIZE32 *= FAT Count
		add DWORD [bp - 8], eax						; DATA_Start += FATSIZE32
		mov eax, DWORD [bp + 0x2C]					; EAX = Root Cluster
	.FATinitilized:

		mov DWORD [bp - 4], eax						; CurrentCluster = Root Cluster
		mov si, BOOTFILE							; SI = Boot File Name
		.FindFile:
			mov di, BUFFER							; DI = Data Buffer
			cmp DWORD [bp - 4], 0					; if(CurrentCluster)
			je .RootDir								; 	Jump to RootDir;
			call FAT_ReadCluster					; Read Cluster into Buffer
			shr cx, 4								; CX = Cluster Size, CX /= 16
			jmp .Search								; Search DIR
			.RootDir:
				mov ebx, DWORD [bp - 12]			; LBA = FAT_Start
				movzx eax, WORD [bp + 0x16]			; EAX = FATSIZE16
				shl eax, 1							; FATSIZE16 *= 2
				add ebx, eax						; LBA += FATSIZE16
				movzx cx, BYTE [bp + 0x0D]			; CX = SectorsPerCluster
				call ReadSectors					; Read Sectors
				shl cx, 4							; SectorsPerCluster *= 16
			.Search:
				.DirLoop:
					pusha							; Save Registers
					mov cx, 11						; CX = 11 (File Name Length 8.3)
					rep cmpsb						; Compare String DS:SI to ES:DI
					popa							; Restore Registers
					je .Found						; If equal we fount the file.
					add di, 0x20					; DIR Entry is 32 Bytes
					loop .DirLoop					; Loop
					jmp ERROR						; ERROR!
			.Found:
				mov ax, WORD [di + 0x14]			; AX = High Cluster
				shl eax, 16							; EAX << 16
				mov ax, WORD [di + 0x1A]			; AX = Low Cluster
				mov DWORD [bp - 4], eax				; CurrentCluster = EAX
		mov di, 0x7E00								; Destination is 0x7E00
		.FATLoadFile:
			call FAT_ReadCluster					; Read Cluster
			add di, cx								; Destination += Cluster Size
			cmp DWORD [bp - 4], 0					; if(CurrentCluster)
			jnz .FATLoadFile						; 	loop;
		mov dl, BYTE [bp - 13]						; DL = bootDrive
		jmp 0x7E00									; Jump to OSLoader
ERROR:
	mov ax, 0x0E45									; Put an E on the Screen
	int 0x10										; With BIOS INT 10h 0Eh
	.halt:
		hlt											; Hault the CPU until interrupt
		jmp .halt									; continue to halt CPU

FAT_ReadCluster:
	mov eax, DWORD [bp - 4]							; EAX = CurrentCluster
	sub eax, 2										; CurrentCluster -= 2
	movzx ecx, BYTE [bp + 0x0D]						; ECX = SectorsPerCluster
	mul ecx											; CurrentCluster *= SectorsPerCluster
	add eax, DWORD [bp - 8]							; CurrentCluster += DATA_Start
	xchg ebx, eax									; EBX = LBA
	call ReadSectors								; ReadSectors
	shl cx, 9										; Length = (SectorsPerCluster * 512)

	pusha											; Save Registers
	mov ebx, DWORD [bp - 12]						; EBX = FAT_Start
	mov edx, DWORD [bp - 4]							; EDX = CurrentCluster
	mov di, WORD FATBUFFER							; DI = FAT Buffer
	mov cx, 2										; Count = 2 (Sectors)

	mov ax, WORD [bp + 0x11]						; AX = Root Entries
	test ax, ax										; if (!Root Entries)
	jz .FAT32										; 	We are FAT32;
	mov ax, WORD [bp + 0x13]						; AX = total secotrs 16
	test ax, ax										; if (!Total Sectors 16)
	jz .FAT16										; 	We are FAT16;
	push dx											; Save DX
	movzx dx, BYTE [bp + 0x0D]						; DX = SectorsPerCluster
	div dx											; Total Sectors 16 /= SectorsPerCluster
	pop dx											; Restore DX
	cmp ax, 0x0FF6									; if(AX > 0x0FF6)
	ja .FAT16										;	We are FAT16;

	.FAT12:
		mov ax, dx									; AX = CurrentCluster
		shr ax, 1									; AX /= 2
		add ax, dx									; AX += CurrentCluster
		mov dx, ax									; DX = AX
		shr dx, 8									; DX >> 8
		add ebx, edx								; LBA (EBX) += EDX
		call ReadSectors							; ReadSectors
		movzx bx, al								; ax cannot be used... bx can...
		mov ax, WORD [di + bx]						; Get Cluster (Kind Of)
		test bl, 1									; Test for Even / Odd Cluster
		jz .FAT12EVEN								; if 0 then EVEN
		shr ax, 4									; Use High 12-bits for ODD
		.FAT12EVEN:
			and ax, 0x0FFF							; Use Low 12-bits for EVEN
		cmp ax, 0x0FF0 								; Check for End of Chain
		jae .EOF									; We fount the End Of File
		jmp .continue								; Continue
	.FAT16:
		movzx ax, dl								; AX = Low 8-Bits of CurrentCluster
		shr dx, 8									; DX >> 8
		add ebx, edx								; LBA (EBX) += EDX
		call ReadSectors							; ReadSectors
		shl ax, 1									; 2 Bytes Per Cluster
		xchg bx, ax									; ax cannot be used... bx can...
		mov ax, WORD [di + bx]						; Get Cluster
		cmp ax, 0xFFF0 								; Check for End of Chain
		jae .EOF									; We found the End Of File
		jmp .continue								; Continue
	.FAT32:
		mov al, dl									; AL = Low 8-Bits of CurrentCluster
		and ax, 0xF									; AND AX 0xF (Only keep low 4-bits)
		shr edx, 4									; EDX >> 4
		add ebx, edx								; LBA (EBX) += EDX
		call ReadSectors							; ReadSectors
		shl ax, 2									; 4 Bytes Per Cluster
		xchg bx, ax									; ax cannot be used... bx can...
		mov eax, DWORD [di + bx]					; Get Cluster
		cmp eax, 0x0FFFFFF0							; Check for End of Chain
		jb .continue								; Continue
	.EOF:
		xor eax, eax								; Clear EAX (New Cluster)
	.continue:
		mov DWORD [bp - 4], eax						; Save New Cluster
	popa											; Restore Registers
	ret												; Return

ReadSectors:
	pusha											; Save Registers
	mov si, DAP										; Setup Disk Address Packet
	mov WORD [si], 0x000C							; DAP Size, and DAP Unused
	mov DWORD [si + 8], ebx							; DAP LBA Low
	mov WORD [si + 6], 0							; Destination in segment 0
	mov WORD [si + 4], di							; DAP Destination Buffer
	mov WORD [si + 2], cx							; DAP Sector Count
	mov dl, BYTE [bp - 13]							; bootDrive
	mov ax, 0x4200									; Extended Read Function
	int 0x13										; Call BIOS
	jc ERROR										; if carry flag is set Error
	popa											; Restore Registers
	ret												; Return

times (499 - ($-$$)) nop							; Padding (or Error)

BOOTFILE db "OSLOADERSYS"							; File Name

dw 0xAA55											; Boot Signiture