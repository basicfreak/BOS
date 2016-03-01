; -----------------------------------------------------------------------------
;                                BOS 0.0.5
;                               BUILD: 0002
;                                 HDD VBR
;                       29/01/2016 - Brian T. Hoover
; -----------------------------------------------------------------------------

bits 16
org 0x7C00

DAP					equ 0x0500
DATA_BUFFER			equ 0x3000
FAT_BUFFER			equ 0x1000


BP_OFF:
	.bootDrive		equ -127
	.FileSize		equ -20
	.FatSector		equ -16
	.FAT_Start		equ -12
	.DATA_Start		equ -8
	.CurrentCluster	equ -4
	.bpb_BPS		equ 0x0B
	.bpb_SPC		equ 0x0D
	.bpb_Reserved	equ 0x0E
	.bpb_FATCount	equ 0x10
	.bpb_RootEnts	equ 0x11
	.bpb_Hidden		equ 0x1C
	.bpb_FatSize 	equ 0x24
	.bpb_RootClust	equ 0x2C

jmp start
times (3 - ($-$$)) nop

times (79) db 0xBB

start:
	mov bp, 0x7C00								; bp points to start of sector
	mov [bp + BP_OFF.bootDrive], dl				; save boot device number

	mov ax, [bp + BP_OFF.bpb_Reserved]			; Calculate Partition Offset
	add eax, [bp + BP_OFF.bpb_Hidden]
	mov [bp + BP_OFF.FAT_Start], eax			; Save FAT Table Start
	mov [bp + BP_OFF.DATA_Start], eax

	mov eax, [bp + BP_OFF.bpb_FatSize]			; EAX = FATSIZE32
	movzx edx, BYTE [bp + BP_OFF.bpb_FATCount]	; EDX = FAT Count
	mul edx										; FATSIZE32 *= FAT Count
	add [bp + BP_OFF.DATA_Start], eax			; DATA_Start += FATSIZE32
	mov eax, [bp + BP_OFF.bpb_RootClust]		; EAX = Root Cluster
	mov [bp + BP_OFF.CurrentCluster], eax		; Save CurrentCluster (RootDIR)

	mov si, BootDir								; FileName = BootDir
	call FAT32_FindFile							; Find File
	mov si, BootDir								; FileName = BootFile
	call FAT32_FindFile							; Find File
	mov ax, 0x1000
	mov es, ax
	mov di, 0x0000								; Destination = 0x1000:0x0000
	call FAT32_ReadFile							; Read File

	jmp 0x8000									; Jump to Boot File

ERROR:
	.halt:
		hlt
		jmp .halt

ReadSectors:
	pusha										; Save Registers
	mov si, DAP									; Setup Disk Address Packet
	mov WORD [si], 0x000C						; DAP Size, and DAP Unused
	mov DWORD [si + 8], ebx						; DAP LBA Low
	mov [si + 6], es							; Destination in segment 0
	mov [si + 4], di							; DAP Destination Buffer
	mov [si + 2], cx							; DAP Sector Count
	mov dl, [bp + BP_OFF.bootDrive]				; boot device number
	mov ax, 0x4200								; Extended Read Function
	int 0x13									; Call BIOS
	jc ERROR									; if carry flag is set Error
	popa										; Restore Registers
	ret											; Return

FAT32_ReadCluster:
	push eax
	push ebx
	push edx
	mov eax, [bp + BP_OFF.CurrentCluster]		; EAX = CurrentCluster
	sub eax, 2									; CurrentCluster -= 2
	movzx ecx, BYTE [bp + BP_OFF.bpb_SPC]		; ECX = SectorsPerCluster
	mul ecx										; CurrentCluster *= SectorsPerCluster
	add eax, [bp + BP_OFF.DATA_Start]			; CurrentCluster += DATA_Start
	xchg ebx, eax
	call ReadSectors							; ReadSectors
	xchg ax, cx									; Length
	mov dx, [bp + BP_OFF.bpb_BPS]				; *= Sector Size
	mul dx
	movzx ecx, ax
	pop edx
	pop ebx
	pop eax
	pushad										; Save Registers
	mov ebx, [bp + BP_OFF.FAT_Start]			; EBX = FAT_Start
	mov edx, [bp + BP_OFF.CurrentCluster]		; EDX = CurrentCluster
	mov di, FAT_BUFFER							; DI = FAT Buffer
	mov cx, 1									; Count = 1 (Sectors)
	mov al, dl									; AL = Low 8-Bits of CurrentCluster
	and ax, 0xF									; AND AX 0xF (Only keep low 4-bits)
	shr edx, 4									; EDX >> 4
	add ebx, edx								; LBA (EBX) += EDX
	call ReadSectors							; ReadSectors
	shl ax, 2									; 4 Bytes Per Cluster
	xchg bx, ax									; ax cannot be used... bx can...
	mov eax, [di + bx]							; Get Cluster
	cmp eax, 0x0FFFFFF0							; Check for End of Chain
	jb .continue								; Continue
	xor eax, eax								; Clear EAX (New Cluster)
	.continue:
		mov [bp + BP_OFF.CurrentCluster], eax	; Save New Cluster
	popad										; Restore Registers
	ret											; Return

FAT32_FindFile:
	pushad
	.ClusterDir:
		mov di, DATA_BUFFER
		call FAT32_ReadCluster
		shr cx, 4
		call .Search
		jnc .Finished
		cmp DWORD [bp + BP_OFF.CurrentCluster], 0
		jnz .ClusterDir
		popad
		stc
		ret
	.Search:
		pusha
		mov cx, 11
		rep cmpsb
		popa
		je .Found
		add di, 0x20
		loop .Search
		stc
		ret
	.Found:
		mov ax, [edi + 0x14] ; High Cluster
		shl eax, 16
		mov ax, [edi + 0x1A] ; Low Cluster
		mov [bp + BP_OFF.CurrentCluster], eax ; CurrentCluster
		ret
	.Finished:
		popad
		ret

FAT32_ReadFile:
	pusha
	mov DWORD [bp + BP_OFF.FileSize], 0
	.readLoop:
		call FAT32_ReadCluster
		add [bp + BP_OFF.FileSize], ecx
		add di, cx
		cmp DWORD [bp + BP_OFF.CurrentCluster], 0
		jnz .readLoop
	popa
	mov ecx, [bp + BP_OFF.FileSize]
	ret

;times (0x01E8 - ($ - $$)) nop

BootDir  db "BOOT       "
BootFile db "SYSINIT BIN"

dw 0xAA55
