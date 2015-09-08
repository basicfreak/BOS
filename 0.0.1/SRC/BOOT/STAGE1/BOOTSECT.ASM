;----------------------------------------------------
;                  BOOTSECT.ASM
;                 (FAT12 FLOPPY)
;            Bootsector Version 0.01.2
;                   2014-07-02
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;          Image Loads in at 07C0:4200
;----------------------------------------------------

[bits 16]
[org 0]

jmp short startup
times 3-($-$$) nop
;----------------------------------------------------
;               BIOS Parameter Block
;----------------------------------------------------
bpbOEM					db "MyOS 0.1"
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 		DB 2
bpbRootEntries: 		DW 224
bpbTotalSectors: 		DW 2880
bpbMedia: 				DB 0xf0  ;; 0xF1
bpbSectorsPerFAT: 		DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 		DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused:               DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MyOS v. 0.1"
bsFileSystem: 	        DB "FAT12   "
;----------------------------------------------------
;                    Entry Point
;----------------------------------------------------
startup:
;----------------------------------------------------
;                   Pointer Setup
;----------------------------------------------------
	cli
	mov ax, 0x07C0
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
	sti
;----------------------------------------------------
;                  Save Boot Device
;----------------------------------------------------
	mov [bootdevice], dl
;----------------------------------------------------
;                 Set Video Mode (3)
;----------------------------------------------------
	mov ah, 0
	mov al, 3
	int 0x10
;----------------------------------------------------
;                    Boot Message
;----------------------------------------------------
	mov si, BOOTMSG
	call puts
;----------------------------------------------------
;                  Initilize FAT12
;----------------------------------------------------
	call initFAT12
;----------------------------------------------------
;                FIND AND LOAD IMAGE
;----------------------------------------------------
	mov bx, 0x4200		; DESTINATION FOR IMAGE
	mov si, ImageName
	call FindFile
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
;                  Sucess Message
;----------------------------------------------------
	mov si, DONEMSG
	call puts
;----------------------------------------------------
;              Store Boot Device in dl
;----------------------------------------------------
	mov dl, [bootdevice]
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
%include "../COMMON/floppy_16.inc"
%include "../COMMON/fat12.inc"
%include "../COMMON/stdio_16.inc"
;----------------------------------------------------
;                     Variables
;----------------------------------------------------
ImageName db "STAGETWOBIN"
BOOTMSG db "Loading Stage Two... ", 0
ERRMSG db "ERROR!", 0xA, 0xD, 0
DONEMSG db "Done!", 0xA, 0xD, 0
bootdevice db 0
ImageSize db 0, 0
;----------------------------------------------------
;                   Boot Signiture
;----------------------------------------------------
times 510-($-$$) db 0
dw 0xAA55
;----------------------------------------------------
;                    FAT TABLE 1
;----------------------------------------------------
db 0xF0
db 0xFF
db 0xFF
times 0x1600-($-$$) db 0
;----------------------------------------------------
;                    FAT TABLE 2
;----------------------------------------------------
db 0xF0
db 0xFF
db 0xFF
times 0x2800-($-$$) db 0
;----------------------------------------------------
;          This Is A Floppy Image 1.44MB
;----------------------------------------------------
times 0x168000-($-$$) db 0
