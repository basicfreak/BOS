; -----------------------------------------------------------------------------
;                                BOS 0.0.5
;                               BUILD: 0002
;                                 HDD MBR
;                       29/01/2016 - Brian T. Hoover
; -----------------------------------------------------------------------------

bits 16
org 0x0600

DAP equ 0x0500

CopyMeDown:
	cli											; Set IF = 0
	xor ax, ax									; Setup data segments (ds, es)
	mov ds, ax
	mov es, ax
	mov ss, ax									; Setup stack
	mov sp, 0xFFF8
	mov di, 0x0600								; MBR Destination
	mov si, 0x7C00								; MBR Source
	mov cx, 0x0100								; MBR WORD count
	rep movsw									; Copy MBR
	jmp 0x0000:start							; Fix CS, Jump to MBR Copy

start:
	sti											; Set IF = 1
	mov BYTE [bootDrive], dl					; Save boot device number
	.CheckPartitions:							; Check Partition Table For Bootable Partition
		mov bx, PT1								; Base = Partition Table Entry 1
		mov cx, 4								; There are 4 Partition Table Entries
		.CKPTloop:
			mov al, BYTE [bx]					; Get Boot indicator bit flag
			test al, 0x80						; Check For Active Bit
			jnz .CKPTFound						; We Found an Active Partition
			add bx, 0x10						; Patrition Table Entry is 16 Bytes
			dec cx								; Decrement Counter
			jnz .CKPTloop						; Loop
		jmp ERROR								; ERROR!
		.CKPTFound:
			mov WORD [PToff], BX				; Save Offset
			add bx, 8							; Increment Base to LBA Address
	.ReadVBR:
		mov EBX, DWORD [bx]						; Start LBA of Active Partition
		mov di, 0x7C00							; We Are Loading VBR to 0x07C0:0x0000
		mov cx, 1								; Only one sector
		call ReadSectors						; Read Sector

	.jumpToVBR:
		mov ax, WORD [0x7DFE]					; Check Boot Signiture
		cmp WORD [0x7DFE], 0xAA55				; Against 0xAA55
		jne ERROR								; Error if not Boot Signiture
		mov si, .msg
		call puts
		mov si, WORD [PToff]					; Set DS:SI to Partition Table Entry
		mov dl, BYTE [bootDrive]				; Set DL to Drive Number
		jmp 0x7C00								; Jump To VBR
		jmp ERROR
	.msg db "BOOT", 0x0A, 0x0D, 0

ERROR:
	mov si, .msg
	call puts
	.halt:
		hlt										; Halt Loop The CPU (with IF=1)
		jmp .halt
	.msg db "ERROR!", 0

ReadSectors:
	pusha										; Save Registers
	mov si, DAP									; Setup Disk Address Packet
	mov WORD [si], 0x000C						; DAP Size, and DAP Unused
	mov DWORD [si + 8], ebx						; DAP LBA Low
	mov WORD [si + 6], es						; Destination in segment 0
	mov WORD [si + 4], di						; DAP Destination Buffer
	mov WORD [si + 2], cx						; DAP Sector Count
	mov dl, BYTE [bootDrive]					; boot device number
	mov ax, 0x4200								; Extended Read Function
	int 0x13									; Call BIOS
	jc ERROR									; if carry flag is set Error
	popa										; Restore Registers
	ret											; Return

puts:
	pusha										; Save Registers
	.StrLoop:
		lodsb									; mov al, BYTE [ds:si]; inc si
		test al, al								; test al for 0
		jz .Done								; Check for Terminator (0)
		mov ah, 0Eh								; Print Char To Screen Function
		int 10h									; Call BIOS
		jmp .StrLoop							; Loop
	.Done:
		popa									; Restore Registers
		ret										; Return

bootDrive db 0									; Our Drive Number Variable
PToff dw 0										; Our Active Partition Offset

times (0x00DA - ($-$$)) nop						; Pad for disk time stamp

DiskTimeStamp times 8 db 'T'					; Disk Time Stamp

times (0x01B4 - ($-$$)) nop						; Pad For MBR Partition Table

UID times 10 db 'P'								; Unique Disk ID
PT1 times 16 db 'A'								; First Partition Entry
PT2 times 16 db 'R'								; Second Partition Entry
PT3 times 16 db 'T'								; Third Partition Entry
PT4 times 16 db 'S'								; Fourth Partition Entry

dw 0xAA55										; Boot Signiture