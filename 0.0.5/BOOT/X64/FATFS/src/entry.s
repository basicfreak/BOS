; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                          Boot Interface - FAT Driver
;                          15/02/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 64
default abs
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

; Functions (RAX)
; 0000h - Open
; 0001h - Read
; 0002h - Write
; 0003h - Close

Entry:
	push rsi
	push rdi
	push rdx
	push rcx
	push r8
	push r9
	push r10
	push r11

	test rax, rax
	jz .Open
	cmp rax, 1
	je .Read
	cmp rax, 2
	je .Write
	cmp rax, 3
	je .Close
	mov rax, 1
	.FunctionReturnAddr:
		clc
		mov [ErrorCode], rax
		test al, al
		jz .return
		stc
		.return:
			pop r11
			pop r10
			pop r9
			pop r8
			pop rcx
			pop rdx
			pop rdi
			pop rsi
			mov rax, [ErrorCode]
			ret
	.Open:
		xchg rdi, rsi
		call FAT_Open
		jmp .FunctionReturnAddr
	.Read:
		call FAT_Read
		jmp .FunctionReturnAddr
	.Write:
		xchg rdi, rcx
		xchg rsi, rbx
		xchg rdx, rsi
		call FAT_Write
		jmp .FunctionReturnAddr
	.Close:
		call FAT_Close
		jmp .FunctionReturnAddr

;int ReadSectors(void*, int, int);
; rdi, esi, edx
; buffer, LBA, length
; 8, 12, 16
ReadSectors:
xchg bx, bx
	push rbx

mov r8, rdi
mov r9, rsi
mov r10, rdx
mov rdi, rsi

	mov rsi, DAP
	mov WORD [rsi], 0xC
	mov [rsi + 2], dx
	mov DWORD [rsi + 4], 0x1000
	mov [rsi + 8], edi


	; mov edi, DAP
	; mov ebx, [ebp + 12]
	; mov ecx, [ebp + 16]
	; mov WORD [edi], 0x000C
	; mov [edi + 2], cx
	; mov DWORD [edi + 4], 0x1000
	; mov [edi + 8], ebx

	mov al, 5
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
		; mov si, DAP
		int 0x13
		jmp .RMReturn
	.CHSmode:
		mov eax, [esi + 8]
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
		mov al, 2
		mov edx, .LMEntry
		jmp AP_Strap

bits 64
	.LMEntry:
xchg bx, bx
		movzx rdx, WORD [esi + 2]
		mov rdi, r8
		mov rsi, 0x1000
		movzx rax, WORD [0x7C00 + BPB.BPS]
		mul rdx
		mov rcx, rax
		rep movsb
	mov QWORD [ErrorCode], 0
	pop rbx
	ret

WriteSectors:
	mov rax, 0
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


section .data

ErrorCode dq 0
aSector db 0
aHead db 0
aTrack db 0
