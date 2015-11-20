[bits 16]
[org 0x0600]

start:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0xFFF0
	.CopyLower:
		mov cx, 0x0100
		mov si, 0x7C00
		mov di, 0x0600
		rep movsw
	mov ax, LowStart
	jmp ax

LowStart:
	clc
	sti
	mov BYTE [bootDrive], dl ; Save BootDrive
	mov si, BootMSG
	call puts
	.CheckPartitions: ; Check Partition Table For Bootable Partition
		mov bx, PT1
		mov cx, 4
		.CKPTloop:
			mov al, BYTE [bx]
			and al, 0x80
			test al, al ; Check For Active Bit
			jnz .CKPTFound
			dec cx
			add bx, 0x10
			test cx, cx
			jnz .CKPTloop
		jmp ERROR
		.CKPTFound:
			mov WORD [PToff], BX
			add bx, 8
	.ReadVBR:
		mov si, DoneMSG
		call puts
		mov si, ReadMSG
		call puts
		mov EBX, DWORD [bx] ; Start LBA of Active Partition
		mov EDX, DWORD 0x00007C00 ; We Are Loading VBR to 0x07C0:0x0000
		mov CX, 1 ; Only one sector
		call ReadSectors

	.jumpToVBR:
		mov ax, WORD [0x7DFE]
		cmp ax, 0xAA55
		jne ERROR
		mov si, DoneMSG
		call puts
		mov si, JmpMSG
		call puts
		mov si, WORD [PToff] ; Make this complient with MS standards
		mov dl, BYTE [bootDrive]
		jmp 0x0000:0x7C00 ; Jump To VBR

ERROR:
	mov si, ErrorMSG
	call puts
	.halt:
		hlt
		jmp .halt

%include "./include/stdio/basic_stdio.inc"
%include "./include/stdio/hdd.inc"

times (218 - ($-$$)) nop      ; Pad for disk timestamp
DiskTimeStamp:
    .zero dw 0
    .OriginalPhysDrive db 0
    .Seconds db 0
    .Minutes db 0
    .Hours db 0

bootDrive db 0
PToff dw 0
BootMSG db "Searching for an Active Partition...", 0
ErrorMSG db "ERROR!", 0
ReadMSG db "Loading VBR...", 0
JmpMSG db "Jumping to VBR...", 0
DoneMSG db "DONE!", 0x0A, 0x0D, 0

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