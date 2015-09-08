;----------------------------------------------------
;                      MBR.S
;              (MBR HDD Boot Sector)
;     Bootsector Version 0.0.3.1 for BOS 0.0.3
;                   2015-08-14
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]
[org 0]

start: ; Standard Setup of stack and pointers
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ax, 0x500
	mov ss, ax
	mov sp, 0x2000
	mov bl, dl

	; Relocate to 0060:0000
	mov si, 0x7C00
	mov di, 0x0600
	mov cx, 256
	xor dx, dx
	rep movsw
	jmp 0x060:StartSub

StartSub: ; Re-Setup Pointers
	mov ax, 0x0060
	mov ds, ax
	mov es, ax
	xor ax, ax
	mov BYTE [bootDrive], bl ; Save BootDrive
	sti
	mov al, 3
	int 0x10 ; Set Video Mode 0x03
	mov si, BootMSG
	call puts

	.CheckPartitions: ; Check Partition Table For Bootable Partition
		mov bx, PT1
		mov cx, 4
		.CKPTloop:
			mov al, BYTE [ds:bx]
			and al, 0x80
			test al, al ; Check For Active Bit
			jnz .CKPTFound
			dec cx
			add bx, 0x10
			test cx, cx
			jnz .CKPTloop
		jmp ERROR1
		.CKPTFound:
			mov WORD [PToff], BX
			add bx, 8
	.ReadVBR:
		mov si, DoneMSG
		call puts
		mov si, ReadMSG
		call puts
		mov EBX, DWORD [ds:bx] ; Start LBA of Active Partition
		mov EDX, DWORD 0x00007C00 ; We Are Loading VBR to 0x07C0:0x0000
		mov CX, 1 ; Only one sector
		call ReadSectors

	.jumpToVBR:
		mov si, DoneMSG
		call puts
		mov si, JmpMSG
		call puts
		mov si, WORD [PToff] ; Make this complient with MS standards
		mov dl, BYTE [bootDrive]
		jmp 0x07C0:0x0000 ; Jump To VBR

ERROR1:
	mov si, Error1MSG
	call puts
	cli
	hlt

ERROR:
	mov si, Error2MSG
	call puts
	cli
	hlt

bootDrive db 0;
PToff dw 0;

BootMSG db "Searching for an Active Partition...", 0
Error1MSG db "ERROR!", 0x0A, 0x0D, "No Partition marked as Active...ABORTING!", 0
Error2MSG db "ERROR!", 0x0A, 0x0D, "Disk Read Error...ABORTING!", 0
ReadMSG db "Loading VBR...", 0
JmpMSG db "Jumping to VBR...", 0
DoneMSG db "DONE!", 0x0A, 0x0D, 0

%include "./COMMON/stdio.inc"

times (0x1b4 - ($-$$)) nop

UID: db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
PT1:
	.bid: db 0
	db 0, 0, 0, 0, 0, 0, 0
	.lba dd 0
	.length dd 0
PT2:
	.bid: db 0
	db 0, 0, 0, 0, 0, 0, 0
	.lba dd 0
	.length dd 0
PT3:
	.bid: db 0
	db 0, 0, 0, 0, 0, 0, 0
	.lba dd 0
	.length dd 0
PT4:
	.bid: db 0
	db 0, 0, 0, 0, 0, 0, 0
	.lba dd 0
	.length dd 0

dw 0xAA55
