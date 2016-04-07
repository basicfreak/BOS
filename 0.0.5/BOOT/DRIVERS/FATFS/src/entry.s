; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                          Boot Interface - FAT Driver
;                          15/02/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 32
; org 0xA000
global Entry
global ReadSectors
global WriteSectors
extern AP_Strap
extern FAT_Open
extern FAT_Read
extern FAT_Write
extern FAT_Close

DAP					equ 0x0500

VAR:
	.bootDrive		equ -127			; BYTE
bootDrive			equ 0x7B81

BPB:
	.BPS			equ 0x0B
	.SPC			equ 0x0D
	.Reserved		equ 0x0E
	.FATCount		equ 0x10
	.RootEnts		equ 0x11
	.Sectors16		equ 0x13
	.FatSize16		equ 0x16
	.Heads			equ 0x1A
	.Hidden			equ 0x1C
	.FatSize32		equ 0x24
	.RootClust		equ 0x2C

; Functions (EAX)
; 0000h - Open
; 0001h - Read
; 0002h - Write
; 0003h - Close

Entry:
	pusha
	test eax, eax
	jz .Open
	cmp eax, 1
	je .Read
	cmp eax, 2
	je .Write
	cmp eax, 3
	je .Close
	mov eax, 1
	.FunctionReturnAddr:
		clc
		mov DWORD [ErrorCode], eax
		test eax, eax
		jz .return
		stc
		.return:
			popa
			mov eax, [ErrorCode]
			ret
	.Open:
		push esi
		call FAT_Open
		add esp, 4
		jmp .FunctionReturnAddr
	.Read:
		push edi
		call FAT_Read
		add esp, 4
		jmp .FunctionReturnAddr
	.Write:
		push ecx
		push ebx
		push esi
		call FAT_Write
		add esp, 12
		jmp .FunctionReturnAddr
	.Close:
		call FAT_Close
		jmp .FunctionReturnAddr

ReadSectors:
	push ebp
	mov ebp, esp
	pusha
	mov edi, DAP
	mov ebx, [ebp + 12]
	mov ecx, [ebp + 16]
	mov WORD [edi], 0x000C
	mov [edi + 2], cx
	mov DWORD [edi + 4], 0x1000
	mov [edi + 8], ebx

	mov al, 1
	mov edx, .RMEntry
	jmp AP_Strap
bits 16
	.RMEntry:
		mov dl, [bootDrive]
		test dl, 0x80
		jz .CHSmode
		clc
		mov ah, 0x41
		mov bx, 0x55AA
		int 0x13
		jc .CHSmode
		bt cx, 0
		jnc .CHSmode
		mov ah, 0x42
		mov si, DAP
		int 0x13
		jmp .RMReturn
	.CHSmode:
		mov eax, [edi + 8]
		call LBAtoCHS
		mov ah, 0x02                     ; read sector
		mov al, 0x01                     ; 1 Sector At A Time
		mov ch, BYTE [aTrack]            ; track
		mov cl, BYTE [aSector]           ; sector
		mov dh, BYTE [aHead]             ; head
		int 0x13
	.RMReturn:
		xor ax, ax
		int 0x13
		xor al, al
		xor ebx, ebx
		mov edx, .PMEntry
		jmp AP_Strap
bits 32
	.PMEntry:
		movzx edx, WORD [esi + 2]
		mov edi, [ebp + 8]
		mov esi, 0x1000
		movzx eax, WORD [0x7C00 + BPB.BPS]
		mul edx
		mov ecx, eax
		rep movsb
		mov [ErrorCode], eax
	popa
	mov eax, [ErrorCode]
	mov DWORD [ErrorCode], 0
	mov esp, ebp
	pop ebp
	ret

bits 16
LBAtoCHS:
	pusha
	mov bp, 0x7C00
	xor dx, dx                           ; prepare dx:ax for operation
	div WORD [bp + BPB.SPC]        ; calculate
	inc dl                               ; adjust for sector 0
	mov BYTE [aSector], dl
	xor dx, dx                           ; prepare dx:ax for operation
	div WORD [bp + BPB.Heads]       ; calculate
	mov BYTE [aHead], dl
	mov BYTE [aTrack], al
	popa
	ret

WriteSectors:
	mov eax, 0
	ret

section .data

ErrorCode dd 0
aSector db 0
aHead db 0
aTrack db 0
