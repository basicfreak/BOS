[bits 16]
[org 0x0600]

DAP equ 0x0800

start:
 cli												; We do not want to be interrupted
	xor ax, ax										; 0 AX
	mov ds, ax										; Set Data Segment to 0
	mov es, ax										; Set Extra Segment to 0
	mov ss, ax										; Set Stack Segment to 0
	mov sp, 0xFFF8									; Set Stack Pointer to 0xFFF8
	.CopyLower:
		mov cx, 0x0100								; 256 WORDs in MBR
		mov si, 0x7C00								; Current MBR Address
		mov di, 0x0600								; New MBR Address
		rep movsw									; Copy MBR
	jmp 0:LowStart									; Jump to new Address

LowStart:
	sti												; Start interrupts
	mov BYTE [bootDrive], dl						; Save BootDrive
	mov si, BootMSG
	call puts
	.CheckPartitions:								; Check Partition Table For Bootable Partition
		mov bx, PT1									; Base = Partition Table Entry 1
		mov cx, 4									; There are 4 Partition Table Entries
		.CKPTloop:
			mov al, BYTE [bx]						; Get Boot indicator bit flag
			test al, 0x80							; Check For Active Bit
			jnz .CKPTFound							; We Found an Active Partition
			add bx, 0x10							; Patrition Table Entry is 16 Bytes
			dec cx									; Decrement Counter
			jnz .CKPTloop							; Loop
		jmp ERROR									; ERROR!
		.CKPTFound:
			mov WORD [PToff], BX					; Save Offset
			add bx, 8								; Increment Base to LBA Address
	.ReadVBR:
		mov si, DoneMSG
		call puts
		mov si, ReadMSG
		call puts
		mov EBX, DWORD [bx]							; Start LBA of Active Partition
		mov di, 0x7C00								; We Are Loading VBR to 0x07C0:0x0000
		mov cx, 1									; Only one sector
		call ReadSectors							; Read Sector

	.jumpToVBR:
		mov ax, WORD [0x7DFE]						; Check Boot Signiture
		cmp WORD [0x7DFE], 0xAA55					; Against 0xAA55
		jne ERROR									; Error if not Boot Signiture
		mov si, DoneMSG
		call puts
		mov si, JmpMSG
		call puts
		mov si, WORD [PToff]						; Set DS:SI to Partition Table Entry
		mov dl, BYTE [bootDrive]					; Set DL to Drive Number
		jmp 0x7C00									; Jump To VBR

ERROR:
	mov si, ErrorMSG
	call puts
	.halt:
		hlt											; Hault the CPU until interrupt
		jmp .halt									; continue to halt CPU

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

puts:
	pusha											; Save Registers
	.StrLoop:
		lodsb										; mov al, BYTE [ds:si]; inc si
		test al, al									; test al for 0
		jz .Done									; Check for Terminator (0)
		mov ah, 0Eh									; Print Char To Screen Function
		int 10h										; Call BIOS
		jmp .StrLoop								; Loop
	.Done:
		popa										; Restore Registers
		ret											; Return

times (218 - ($-$$)) nop							; Pad for disk time stamp

DiskTimeStamp times 8 db 0							; Disk Time Stamp

bootDrive db 0										; Our Drive Number Variable
PToff dw 0											; Our Partition Table Entry Offset
BootMSG db "Searching for an Active Partition...", 0
ErrorMSG db "ERROR!", 0
ReadMSG db "Loading VBR...", 0
JmpMSG db "Jumping to VBR...", 0
DoneMSG db "DONE!", 0x0A, 0x0D, 0

times (0x1b4 - ($-$$)) nop							; Pad For MBR Partition Table

UID times 10 db 0									; Unique Disk ID
PT1 times 16 db 0									; First Partition Entry
PT2 times 16 db 0									; Second Partition Entry
PT3 times 16 db 0									; Third Partition Entry
PT4 times 16 db 0									; Fourth Partition Entry

dw 0xAA55