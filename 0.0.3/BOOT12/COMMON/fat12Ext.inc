;----------------------------------------------------
;                  fat12Ext.inc
;                   2015-07-06
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __FAT12_INC__INCLUDED__
%define __FAT12_INC__INCLUDED__

[bits 16]

;OFFSET should = 0BE0

;Cluster Table is stored at OFFSET:7C00 - OFFSET:7FFF (2 sectors on floppy [1024 Bytes])
;Real Addr should = 13A00 - 13DFF

;Temporary data storage at OFFSET:8000 - OFFSET:FFFF (64 sectors on floppy [32 Kilo Bytes])
;Real Addr should = 13E00 - 1BDFF

;Current Directory Stored at OFFSET:5400 - OFFSET:7BFF
; 11200 - 139FF


;----------------------------------------------------
;                    nextCluster
;   INPUT: WORD [CurrentCluster] = current cluster
;    OUTPUT: WORD [CurrentCluster] = next cluster
;----------------------------------------------------
nextCluster:
;----------------------------------------------------
;                   Save Registers
;----------------------------------------------------
	pusha
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx

	mov     ax, WORD [CurrentCluster]	; identify current cluster
	mov     cx, ax						; copy current cluster
	mov     dx, ax
	shr     dx, 0x0001					; divide by two
	add     cx, dx						; sum for (3/2)
	; CX = Current Cluster Location In FAT
	; AX = CurrentCluster
	mov bx, cx
	shr bx, 9	; Divide by 512
;and bx, 0xFFFE	; Make sure we are on even sector in FAT
	mov dx, WORD [bpbReservedSectors]
	add dx, bx
	; DX = what CurrentClustSect needs to be
	mov bx, WORD [CurrentClustSect]
	cmp dx, bx
	je .clusterLoaded
	pusha
	mov cx, 2	; Read 2 sectors
	mov ax, dx	; Start at DX
	mov bx, 0x7C00	; to ES:7C00
	call ReadSectors
	jc ERROR_STUB
	popa
	mov WORD [CurrentClustSect], dx		; Set CurrentClustSect
	.clusterLoaded:
		push ax
		mov bx, WORD [bpbReservedSectors]
		shl bx, 9 ; Multiply by 512
		add bx, cx	; BX = Cluster Location on Floppy (in Bytes)
		mov ax, WORD [CurrentClustSect]
		shl ax, 9
		sub bx, ax
		mov ax, 0x7C00
		add bx, ax
		pop ax
	; BX = Offset in data area (hopefully)
	mov		dx, WORD [es:bx]
	test	ax, 0x0001					; test for odd or even cluster
	jnz		.ODD_CLUSTER
	.EVEN_CLUSTER:
		and		dx, 0000111111111111b	; take low 12 bits
		jmp		.DONE
	.ODD_CLUSTER:
		shr		dx, 0x0004				; take high 12 bits
	.DONE:
;xchg bx, bx
		mov		WORD [CurrentCluster], dx
;----------------------------------------------------
;                 Restore Registers
;----------------------------------------------------
	popa
;----------------------------------------------------
;                      Return
;----------------------------------------------------
	ret

; Output Root Dir ES:8000
ReadRootDir:
	pusha
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	
	mov cx, WORD [bpbRootEntries]	
	shr cx, 4 ; Divide by 16
	mov dx, cx
	shl dx, 9
	add dx, 0x8000
	mov WORD [DataBufferEnd], dx
	
	push cx
	xor cx, cx
	mov ax, WORD [bpbSectorsPerFAT]
	mov cl, BYTE [bpbNumberOfFATs]
	mul cl
	mov dx, WORD [bpbReservedSectors]
	add ax, dx
	pop cx

;	mov bx, 0x8000	; to ES:8000
	mov bx, 0x5400

	call ReadSectors
	jc ERROR_STUB
	popa
	ret

; Input CurrentCluster
; 		EAX - 0 for buffer only else address for data
; Output ES:8000 32KB Buffer
ReadFile:
	pusha
	db 0x66, 0x67
		push eax
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	mov bp, 0

	
	mov bx, 0x8000
	.readCluster:
		mov cx, 0
		mov cl, BYTE [bpbSectorsPerCluster]
		mov ax, WORD [bpbSectorsPerFAT]
		mov dl, BYTE [bpbNumberOfFATs]
		mul dl
		mov dx, WORD [bpbReservedSectors]
		add ax, dx
		mov dx, WORD [bpbRootEntries]
		shr dx, 4
		add ax, dx
		mov dx, WORD [CurrentCluster]
		sub dx, 2
		add ax, dx
		call ReadSectors
		jc ERROR_STUB
		inc bp
		call nextCluster
		mov dx, cx
		shl dx, 9
		add bx, dx
		mov WORD [DataBufferEnd], bx


		cmp bx, 0xFE00
		jge .RFCpy


		mov dx, WORD [CurrentCluster]
		cmp dx, 2
		jle ERROR_STUB
		cmp dx, 2
		jle .RFDone
		cmp dx, 0xFF8
		jl .readCluster

	db 0x66, 0x67
		pop eax
	db 0x66, 0x67
		cmp eax, 0
	je .RFDone
	db 0x66, 0x67
		push eax
.RFCpy:
;xchg bx, bx
	db 0x66, 0x67
		pop eax
	db 0x66, 0x67
		mov esi, 0x13E00
	db 0x66, 0x67
		mov ecx, 0
	mov cx, WORD [DataBufferEnd]
	db 0x66, 0x67
		sub ecx, 0x8000
	db 0x66, 0x67
		push ecx
	db 0x66, 0x67
		shr ecx, 2
	db 0x66, 0x67
		mov edi, eax
	call memcpy
	db 0x66, 0x67
		pop ecx
	mov dx, WORD [CurrentCluster]
	cmp dx, 0xFF8
	jge .RFDone
	db 0x66, 0x67
		add eax, ecx
	mov bx, 0x8000
	mov WORD [DataBufferEnd], bx
	;db 0x66, 0x67
	;	shl ecx, 2
	;db 0x66, 0x67
	;	add eax, ecx
	db 0x66, 0x67
		push eax
	jmp .readCluster

	.RFDone:
		mov WORD [CurFileSectorCount], bp
		popa
		ret


; Input si - File Name
; Output CurrentCluster - Start Cluster
FindFileInDir:
	pusha
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	
	mov cx, WORD [DataBufferEnd]
;	sub cx, 0x8000
	sub cx, 0x5400
	shr cx, 5
;	mov di, 0x8000
	mov di, 0x5400
	mov bx, si
	.FileDirLoop:
		push    cx
		mov     cx, 11					; eleven character name. Image name is in SI
		mov		si, bx					; image name is in BX
		push    di
		rep		cmpsb					; test for entry match
		pop     di
		pop     cx
		je      .FileDirFound
		add     di, 32					; queue next directory entry
		loop .FileDirLoop
	popa
	mov ax, -1
	ret
	.FileDirFound:
		mov dx, WORD [es:di + 0x001A];  ; start cluster		
		mov WORD [CurrentCluster], dx
		popa
		mov ax, 1
		ret

CurrentCluster		dw 0
CurrentClustSect	dw 0
DataBufferEnd		dw 8000
TempData			dw 0
CurFileSectorCount	dw 0

%endif		;__FAT12_INC__INCLUDED__