;----------------------------------------------------
;                   fat12.inc
;                   2014-07-02
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __FAT12_INC__INCLUDED__
%define __FAT12_INC__INCLUDED__

[bits 16]

;----------------------------------------------------
;                    initFAT12
;         No inputs! This is set in stone!
;          Reads Sector 1-33 (base 0) to:
;              07C0:0200 - 07C0:41FF
;----------------------------------------------------
initFAT12:
	pusha
	mov cx, 32
	mov ax, 1
	mov bx, 0x200
	call ReadSectors
	popa
	ret

;----------------------------------------------------
;                    nextCluster
;      INPUT: WORD [cluster] = current cluster
;       OUTPUT: WORD [cluster] = next cluster
;----------------------------------------------------
nextCluster:
;----------------------------------------------------
;                   Save Registers
;----------------------------------------------------
	pusha
	push es
	mov ax, 0x07C0
	mov es, ax
	mov     ax, WORD [cluster]			; identify current cluster
	mov     cx, ax						; copy current cluster
	mov     dx, ax
	shr     dx, 0x0001					; divide by two
	add     cx, dx						; sum for (3/2)
	mov		bx, 0x200					; FAT offset
	add		bx, cx
	mov		dx, WORD [es:bx]
	test	ax, 0x0001					; test for odd or even cluster
	jnz		.ODD_CLUSTER
	.EVEN_CLUSTER:
		and		dx, 0000111111111111b	; take low 12 bits
		jmp		.DONE
	.ODD_CLUSTER:
		shr		dx, 0x0004				; take high 12 bits
	.DONE:
		mov		WORD [cluster], dx
;----------------------------------------------------
;                 Restore Registers
;----------------------------------------------------
	pop es
	popa
;----------------------------------------------------
;                      Return
;----------------------------------------------------
	ret

;----------------------------------------------------
;                     FindFile
; Due To Limited Space in Boot Sector This Will Find
;   And Load The File Into The Buffer All At Once.
;        INPUT: SI = FILENAME (7.3 - no '.')
;               INPUT: ES:BX = BUFFER
;             OUTPUT: AX = -1 on error
;        OUTPUT: CX = FILE SIZE in CLUSTERS
;----------------------------------------------------
FindFile:
;----------------------------------------------------
;                 LETS FIND THE FILE
;----------------------------------------------------
;----------------------------------------------------
;                   Save Registers
;----------------------------------------------------
	pusha
	mov		bx, si						; copy filename for later
	mov     cx, WORD [bpbRootEntries]	; loop register
	mov     di, 0x2600					; root dir mem location
	cld
	.LOOP:
		push    cx
		mov     cx, 11					; eleven character name. Image name is in SI
		mov		si, bx					; image name is in BX
		push    di
		rep		cmpsb					; test for entry match
		pop     di
		je      .FOUND
		pop     cx
		add     di, 32					; queue next directory entry
		loop    .LOOP
;----------------------------------------------------
;                      ERROR
;----------------------------------------------------
	.NOTFOUND:
;----------------------------------------------------
;                 Restore Registers
;----------------------------------------------------
		popa
		mov	ax, -1						; set error code
;----------------------------------------------------
;                      Return
;----------------------------------------------------
		ret
;----------------------------------------------------
;                      SUCESS
;----------------------------------------------------
	.FOUND:
		mov		dx, WORD [es:di + 0x001A];  ; start cluster		
		mov		WORD [cluster], dx	
		pop ax							; Not sure why I need this?!? As I dont think I ever pushed AX onto stack
										; nor anything else not already pulled off
;----------------------------------------------------
;                 Restore Registers
;----------------------------------------------------
		popa
;----------------------------------------------------
;                 LETS LOAD THE FILE
;----------------------------------------------------
;----------------------------------------------------
;                   Save Registers
;----------------------------------------------------
	push	cx
	push	dx
	
	xor	cx, cx		; size of file in sectors
	push	cx
	
	.ReadLoopMe:
		mov		si, dotMSG
		call	puts
		mov 	ax, WORD [cluster]			; cluster number
		add		ax, 31						; sector offset
		mov		cx, 1						; read one sector
		push 	bx
		call	ReadSectors					; read
		call	nextCluster					; get next cluster		
		pop		bx
		add		bx, 0x200					; add 512 bytes to buffer location
		mov		ax, WORD [cluster]
		
		pop		cx
		inc		cx			; add one more sector to counter
		push	cx
		
		
		cmp		ax, 0xff8
		jl		.ReadLoopMe
	pop	cx
;----------------------------------------------------
;                 Restore Registers
;----------------------------------------------------
	pop	dx	
	pop cx
	
	mov	ax, 1
;----------------------------------------------------
;                      Return
;----------------------------------------------------
	ret

	dotMSG db ".", 0

%endif		;__FAT12_INC__INCLUDED__
