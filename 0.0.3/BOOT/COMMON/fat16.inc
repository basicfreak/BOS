;----------------------------------------------------
;                  fat16Ext.inc
;                   2015-08-13
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __FAT16_INC__INCLUDED__
%define __FAT16_INC__INCLUDED__

[bits 16]

;NOTE : THIS IS ASSUMING THE FILE WE ARE LOOKING FOR
;       IS IN THE FIRST SECTOR OF THE ROOT DIR AND IN
;       THE FIRST SECTOR OF THE FILE ALLOCATION TABLE!
initFAT16:
	pusha
	; COPY FIRST SECTOR OF FAT TO 07C0:200
	mov cx, 1
	mov ax, WORD [bpbReservedSectors]
	mov bx, 0x200
	call ReadSectors
	; COPY FIRST SECTOR OF ROOTDIR TO 07C0:400
	mov dx, WORD [bpbSectorsPerFAT]
	shl dx, 1 ; Multiply by two as there are ALWAYS 2 FATs never more never less
	add cx, dx
	mov bx, 0x400
	call ReadSectors
	popa
	ret

; Input si - File Name
; Output CurrentCluster - Start Cluster
FindFileInDir:
	pusha
	mov cx, 0x10	; There are 16 DIR Entries in the sector
	mov di, 0x400
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

; Input CurrentCluster
; Output CurrentCluster
NextCluster:
	pusha
	mov bx, 0x200
	mov es, bx
	mov bx, WORD [CurrentCluster]			; identify current cluster
	shr bx, 0x0001					; divide by two
	mov dx, WORD [es:bx]
	mov WORD [CurrentCluster], dx
	popa
	ret


; Input si - filename
; output es:bx Buffer
LoadFile:
	pusha
	call FindFileInDir
	mov cx, 0
	mov cl, BYTE [bpbSectorsPerCluster]
	mov dx, cx
	shl dx, 9 ; There are 512 bytes per sector on all HDDs
	.FileLoop:
;mov cx, 1
;mov ax, WORD [bpbReservedSectors]
;mov bx, 0x200
;call ReadSectors
		mov ax, WORD [bpbSectorsPerFAT]
		shl ax, 1
		add ax, WORD [bpbReservedSectors]
		push cx
		imul cx, WORD [CurrentCluster]; Maybe?
		add ax, cx
		pop cx
		call ReadSectors
		call NextCluster
		add bx, dx
		cmp WORD [CurrentCluster], 0xFFF0
		jl .FileLoop
	popa
	ret

CurrentCluster		dw 0

%endif		;__FAT16_INC__INCLUDED__