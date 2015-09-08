;----------------------------------------------------
;                      VBR.S
;             (VBR FAT16 Boot Sector)
;     Bootsector Version 0.0.3.1 for BOS 0.0.3
;                   2015-08-14
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;          Image Loads in at 07C0:4200
;----------------------------------------------------

[bits 16]
[org 0]

jmp start

bootDrive db 0
times (3-($-$$)) db 0x90 ; Just so we fail if we are past 3 bytes here
;----------------------------------------------------
;               BIOS Parameter Block
;----------------------------------------------------
bpbOEM:					db 0, 0, 0, 0, 0, 0, 0, 0
bpbBytesPerSector:  	DW 0
bpbSectorsPerCluster: 	DB 0
bpbReservedSectors: 	DW 0
bpbNumberOfFATs: 		DB 0
bpbRootEntries: 		DW 0
bpbTotalSectors: 		DW 0
bpbMedia: 				DB 0
bpbSectorsPerFAT: 		DW 0
bpbSectorsPerTrack: 	DW 0
bpbHeadsPerCylinder: 	DW 0
bpbHiddenSectors: 		DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused:               DB 0
bsExtBootSignature: 	DB 0
bsSerialNumber:	        DD 0
bsVolumeLabel: 	        DB 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
bsFileSystem: 	        DB 0, 0, 0, 0, 0, 0, 0, 0

;----------------------------------------------------
;                 Beginning Of Code
;----------------------------------------------------
start: ; Set Pointers and save data
xchg bx, bx
	cli
	mov ax, 0x07C0
	mov es, ax
	add si, 8 ; LBA Offset in Partition Table
	mov ebx, DWORD [ds:si] ; Store LBA in EBX
	mov cx, ds ; copy ds to cx
	mov ds, ax
	mov BYTE [bootDrive], dl ; Save Boot Drive
	mov DWORD [PTOffset], ebx ; Store Start LBA in DWORD PTOffset
	mov WORD [PTDS], cx ; Save old DS
	sub si, 8
	sti

; NOTE IF NOT LODING FROM BOS MBR UNCOMMENT THESE: (May Not Be Room so instead uncomment the first three here and comment out all mov si, X \n call puts)
;	mov ax, 0x500
;	mov ss, ax
;	mov sp, 0x2000
;	mov ax, 3
;	int 0x10

push si ; Moving si into a variable failed so lets just push it
	mov si, DoneMSG
	call puts

	mov si, IMAGENAME
	call FindFileInRoot
	jc ERROR
	mov EDX, DWORD 0xBE00
	call LoadFile
;----------------------------------------------------
;         Store Important BPB info to stack
;----------------------------------------------------
pop si
	mov al, BYTE [bpbSectorsPerCluster]
	push ax
	push WORD [bpbReservedSectors]
	push WORD [bpbRootEntries]
	push WORD [bpbSectorsPerFAT]
	push DWORD [bpbHiddenSectors]
	
;----------------------------------------------------
;               Jump To OSLOADERSYS
;----------------------------------------------------
	mov dl, BYTE [bootDrive]
	mov ds, WORD [PTDS]
	jmp 0x0BE0:0x0000

ERROR:
	mov si, ERRMSG
	call puts
	cli
	hlt

%include "./COMMON/stdio.inc"
%include "./COMMON/fat16.inc"

DoneMSG db "DONE!", 0x0A, 0x0D, "Loading "
IMAGENAME db "OSLOADERSYS" ; Sounds so much better than stagetwo.bin
db "...", 0 ; See what I did there ;)
ERRMSG db "ERROR", 0

PTOffset dd 0
PTDS dw 0

times (0x1FE - ($-$$)) nop
dw 0xAA55
