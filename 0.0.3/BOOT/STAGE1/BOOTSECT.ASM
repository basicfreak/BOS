;----------------------------------------------------
;                  BOOTSECT.ASM
;                 (FAT12 FLOPPY)
;          Bootsector Version 0.0.2.1
;                   2015-07-04
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;          Image Loads in at 07C0:4200
;----------------------------------------------------

[bits 16]
[org 0]
;[org 0x7c00]

jmp short startup
times 3-($-$$) nop
;----------------------------------------------------
;               BIOS Parameter Block
;----------------------------------------------------
bpbOEM					DB "        "
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
bsVolumeLabel: 	        DB "           "
bsFileSystem: 	        DB "        "
;----------------------------------------------------
;                    Entry Point
;----------------------------------------------------
startup:
;----------------------------------------------------
;                   Pointer Setup
;----------------------------------------------------
	cli
	mov ax, 0x07C0
	;xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
;----------------------------------------------------
;                    Stack Setup
;----------------------------------------------------
	mov ax, 0x500
	mov ss, ax
	mov sp, 0x2000
;----------------------------------------------------
;                  Save Boot Device
;----------------------------------------------------
	mov [bootdevice], dl
	sti
;----------------------------------------------------
;                 Set Video Mode (3)
;----------------------------------------------------
	mov ah, 0
	mov al, 3
	int 0x10
	mov si, BOOTINGMSG
	call puts
;----------------------------------------------------
;                  Initilize FAT12
;----------------------------------------------------
	call initFAT16
;----------------------------------------------------
;                FIND AND LOAD IMAGE
;----------------------------------------------------
	mov bx, 0x4200		; DESTINATION FOR IMAGE
	mov si, ImageName
	call LoadFile
;----------------------------------------------------
;            Check If We Loaded The Image
;----------------------------------------------------
	cmp ax, 1
	je jumpImg
;----------------------------------------------------
;        ERROR MESSAGE (We didn't load Image)
;----------------------------------------------------
ErrorSub:
	mov si, ERRMSG
	call puts
	cli
	hlt
	jmp $				; If We Get Here Infinite Loop
;----------------------------------------------------
;      We Loaded the Image time to jump to it
;----------------------------------------------------
jumpImg:
;----------------------------------------------------
;              Store Boot Device in dl
;----------------------------------------------------
	mov dl, [bootdevice]
;----------------------------------------------------
;         Store Important BPB info to stack
;----------------------------------------------------
	xor ax, ax
	push WORD [bpbBytesPerSector]
	mov al, BYTE [bpbSectorsPerCluster]
	push ax
	push WORD [bpbReservedSectors]
	mov al, BYTE [bpbNumberOfFATs]
	push ax
	push WORD [bpbRootEntries]
	push WORD [bpbTotalSectors]
	mov al, BYTE [bpbMedia]
	push ax
	push WORD [bpbSectorsPerFAT]
	push WORD [bpbSectorsPerTrack]
	push WORD [bpbHeadsPerCylinder]
	push DWORD [bpbHiddenSectors]
	push DWORD [bpbTotalSectorsBig]
	mov al, BYTE [bsDriveNumber]
	push ax
	mov al, BYTE [bsExtBootSignature]
	push ax

;----------------------------------------------------
;               Time to jump to IMAGE!
;----------------------------------------------------
	push WORD 0x0BE0
	push WORD 0x0000
	retf
;----------------------------------------------------
;                If We Get Here ERROR!
;----------------------------------------------------
	jmp ErrorSub
;----------------------------------------------------
;                 Drivers / Includes
;----------------------------------------------------
%include "./COMMON/floppy_16.inc"
%include "./COMMON/fat16.inc"
%include "./COMMON/stdio_16.inc"
;----------------------------------------------------
;                     Variables
;----------------------------------------------------
ImageName db "STAGETWOBIN"
ERRMSG db "ERROR", 0
BOOTINGMSG db "Loading Stage Two...", 0
bootdevice db 0
;----------------------------------------------------
;                   Boot Signiture
;----------------------------------------------------
times 510-($-$$) db 0
dw 0xAA55
