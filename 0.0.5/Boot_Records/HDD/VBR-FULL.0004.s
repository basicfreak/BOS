; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                              HDD VBR (FAT 16/32)
;                          08/02/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 16
org 0x7C00

DAP					equ 0x0500
BUFFER				equ 0x1000			; 16 KB
FATBUFFER			equ 0x5000			;  4 KB

VAR:
	.bootDrive		equ -127			; BYTE
	.bootInfo		equ -126			; BITFIELD
		; 13/42 | 13/02 | PXE | ? | TFTP | ? | ExtFS | FATFS
	.FAT_Start		equ -12				; DWORD
	.DATA_Start		equ -8				; DWORD
	.RootClust		equ -4				; DWORD

BPB:
	.BPS			equ 0x0B
	.SPC			equ 0x0D
	.Reserved		equ 0x0E
	.FATCount		equ 0x10
	.RootEnts		equ 0x11
	.Sectors16		equ 0x13
	.FatSize16		equ 0x16
	.Hidden			equ 0x1C
	.FatSize32		equ 0x24
	.RootClust		equ 0x2C

jmp start

times (3 - ($ - $$)) nop

times (79) db 0xBB

start:
	mov bp, 0x7C00						; bp points to start of sector
	mov [bp + VAR.bootDrive], dl		; bootDrive
	mov [bp + VAR.bootInfo], BYTE 0x81	; 13h/42h | FATFS

	movzx dx, BYTE [bp + BPB.SPC]		; Make sure Cluster size <= 16KB
	mov ax, [bp + BPB.BPS]
	mul dx								; SectorsPerCluster * BytesPerSector
	cmp ax, 0x2000
	ja ERROR

	movzx eax, WORD [bp + BPB.Reserved]	; ReservedSectors
	add eax, [bp + BPB.Hidden]			; HiddenSectors
	mov [bp + VAR.FAT_Start], eax		; FAT_Start
	mov [bp + VAR.DATA_Start], eax		; DATA_Start
	movzx eax, WORD [bp + BPB.FatSize16]; EAX = FATSIZE16
	test ax, ax							; Does this value exist
	jz .FAT32							; No, This is FAT32
	; Ok we are either FAT12 or FAT16...

	mov cx, [bp + BPB.Sectors16]		; CX = total secotrs 16
	jcxz .FAT16							; 	We are FAT16;
	xchg ax, cx
	movzx dx, BYTE [bp + BPB.SPC]		; DX = SectorsPerCluster
	div dx								; Total Sectors 16 /= SectorsPerCluster
	cmp ax, 0x0FF6						; if(AX <= 0x0FF6)
	jbe ERROR							;	We are FAT12, ERROR!
	xchg ax, cx

	.FAT16:
		shl ax, 1						; Multiply FATSIZE16 by 2
		mov dx, [bp + BPB.RootEnts]		; DX = Root Entries
		shr dx, 4						; Root Entries /= 16
		add ax, dx						; FATSIZE16 += Root Entries
		xor ecx, ecx					; No Root Cluster, 0 ECX
		jmp .FATinitilized				; We are finished here
	.FAT32:
		mov eax, [bp + BPB.FatSize32]	; EAX = FATSIZE32
		movzx edx, BYTE[bp+BPB.FATCount]; EDX = FAT Count
		mul edx							; FATSIZE32 *= FAT Count
		mov ecx, [bp + BPB.RootClust]	; EAX = Root Cluster
	.FATinitilized:
		add [bp + VAR.DATA_Start], eax	; DATA_Start += FATSIZE
		mov [bp + VAR.RootClust], ecx	; CurrentCluster = Root Cluster

	mov si, BOOTPATH					; Path = BOOTPATH
	mov cx, 2							; Path Depth = 2
	mov di, 0x8000						; Destination = 0x7E00
	call ReadFile						; Read File
	jc ERROR
	jmp 0x8000							; Jump to File

ERROR:
	mov si, ERROR_MSG
	.StrLoop:
		lodsb							; mov al, BYTE [ds:si]; inc si
		test al, al						; test al for 0
		jz .Done						; Check for Terminator (0)
		mov ah, 0Eh						; Print Char To Screen Function
		int 10h							; Call BIOS
		jmp .StrLoop					; Loop
	.Done:
		hlt
		jmp .Done
	
ReadSectors:
	pusha								; Save Registers
	mov si, DAP							; Setup Disk Address Packet
	mov WORD [si], 0x000C				; DAP Size, and DAP Unused
	mov [si + 8], ebx					; DAP LBA Low
	mov [si + 6], es					; Destination in segment 0
	mov [si + 4], di					; DAP Destination Buffer
	mov [si + 2], cx					; DAP Sector Count
	mov dl, [bp + VAR.bootDrive]		; bootDrive
	mov ah, 0x42						; Extended Read Function
	int 0x13							; Call BIOS
	popa								; Restore Registers
	ret									; Return

times (0x100 - ($-$$)) nop

; cx = depth
; ds:si = path
; es:di = dest
ReadFile:
	pushad								; Save all Registers
	push di
	push cx								; Save CX
	mov eax, [bp + VAR.RootClust]		; Current Cluster = Root Cluster
	test eax, eax						; If RootClust == 0 (FAT16 RootDir)
	jz .RootDir
	.DirDepthLoop:
		mov di, BUFFER					; Destination = Buffer
		call FAT_ReadCluster			; Read Cluster
		jc .Done
		shr cx, 4						; Cluster Size / 16
	.Search:
		pusha							; Save Registers
		mov cx, 11						; 8.3 File Name (11 Bytes)
		rep cmpsb						; Compare File Name
		popa							; Restore Registers
		je .Found						; If Equal we found File Name
		add di, 0x20					; DIR Ent = 0x20 Bytes
		loop .Search					; Loop DIR
	.NotFound:
		pop di
		stc								; Failed to find File Name
	.Found:
		pop cx							; Restore CX
		jc .Done						; Return on Fail
		mov ax, [di + 0x14]				; AX = High Cluster
		shl eax, 16						; EAX << 16
		mov ax, [di + 0x1A]				; AX = Low Cluster
		dec cx
		jcxz .RF1						; Read File if done searching
		push cx							; Save CX
		add si, 11
		jmp .DirDepthLoop				; Search Next Dir
	.RF1:
		pop di
	.ReadFile:
		call FAT_ReadCluster			; Read Cluster
		jc .Done						; Return on Fail
		add di, cx						; Destination = Dest + Cluster Size
		test eax, eax					; if(CurrentCluster)
		jnz .ReadFile					; 	loop;
	.Done:
		popad							; Restore all Registers
		ret								; Return
	.RootDir:
		mov di, BUFFER					; Destination = Buffer
		mov ebx, [bp + VAR.FAT_Start]	; LBA = FAT_Start
		movzx eax,WORD[bp+BPB.FatSize16]; EAX = FATSIZE16
		shl eax, 1						; FATSIZE16 *= 2
		add ebx, eax					; LBA += FATSIZE16
		movzx cx, BYTE [bp + BPB.SPC]	; CX = SectorsPerCluster
		call ReadSectors				; Read Sectors
		jc .Done						; Return on Fail
		shl cx, 4						; SectorsPerCluster *= 16
		jmp .Search


; INPUT:
; EAX = CurrentCluster
; ES:DI = BUFFER

; OUTPUT:
; EAX = Next Cluster
; CX = Byte Count Read

; PERSERVED:
; DI
FAT_ReadCluster:
	push eax							; Save Current Cluster
	sub eax, 2							; Cluster -= 2
	movzx ecx, BYTE [bp + BPB.SPC]		; Cluster *= SectorsPerCluster
	mul ecx
	add eax, [bp + VAR.DATA_Start]
	xchg eax, ebx						; EBX = LBA
	call ReadSectors					; Read Cluster
	shl cx, 9							; Cluster Sectors * 512 Bytes
	pop eax								; Restore Current Cluster
	jc .ErrRet
	push cx								; Save Cluster Size
	push di								; Save Destination

	mov ebx, [bp + VAR.FAT_Start]		; FAT LBA Start
	mov di, FATBUFFER					; FAT Buffer Destination

	push WORD 1							; Save Sector Count

	movzx dx, al						; Save low 8 Bits as offset
	mov cx, [bp + BPB.RootEnts]			; Check for FAT32 or FAT16
	jcxz .FAT32

	.FAT16:
		shr ax, 8						; Remove low 8 Bit from Sector
		add ebx, eax					; Add Sector to LBA
		pop cx							; Restore Sector Count
		call ReadSectors				; ReadSectors
		shl dx, 1						; 2 Bytes Per Cluster
		xchg bx, dx						; ax cannot be used... bx can...
		mov ax, [di + bx]				; Get Cluster
		cmp ax, 0xFFF0 					; Check for End of Chain
		jae .EOF						; We found the End Of File
		jmp .Done						; Continue
	.FAT32:
		and dx, 0xF						; Only 4 Bit Offset
		shr eax, 4						; Remove low 4 Bit from Sector
		add ebx, eax					; Add sector to LBA
		pop cx							; Restore Sector Count
		call ReadSectors				; ReadSectors
		shl dx, 2						; 4 Bytes Per Cluster
		xchg bx, dx						; dx cannot be used... bx can...
		mov eax, [di + bx]				; Get Cluster
		cmp eax, 0x0FFFFFF0				; Check for End of Chain
		jb .Done						; Continue
	.EOF:
		xor eax, eax					; Clear EAX (New Cluster)
	.Done:
		clc
		pop di							; Restore Registers
		pop cx
	.ErrRet:
		ret								; Return

ERROR_MSG db "BOOT: Failed", 0

times (0x01E8 - ($ - $$)) db 0

BOOTPATH:
BOOTDIR  db "BOOT       "				; Boot Folder
BOOTFILE db "SYSINIT BIN"				; Boot File

dw 0xAA55								; Boot Signiture
