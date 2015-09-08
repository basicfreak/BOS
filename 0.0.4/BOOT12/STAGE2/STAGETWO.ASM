;----------------------------------------------------
;                  STAGETWO.ASM
;                 (FAT12 FLOPPY)
;            Bootsector Version 0.01.2
;                   2014-07-04
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;           Kernel Loads in at 0x100000
;----------------------------------------------------

[bits 16]
[org 0xBE00]

jmp short startup
nop

;----------------------------------------------------
; BIOS Parameter Block
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

startup:
;----------------------------------------------------
;    Pointer Setup (STAGEONE loads us to 0xBE00)
;---------------------------------------------------- 
	cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax	
;----------------------------------------------------
;                    create stack
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
;                  Display Message
;----------------------------------------------------
	mov si, msg
	call puts



	mov al, 0x0C
	mov dx, 0x3F2
	out dx, al
	cli
	hlt
	jmp $
;----------------------------------------------------
;                 Drivers / Includes
;----------------------------------------------------
%include "../COMMON/floppy_16.inc"
%include "../COMMON/fat12.inc"
%include "../COMMON/stdio_16.inc"
;----------------------------------------------------
;                     Variables
;----------------------------------------------------
KERNEL db "KERNEL ELF"
msg db "Preloading Kernel... ", 0
ERRMSG db "ERROR!",0xA, 0xD, 0
DONEMSG db "Done!", 0xA, 0xD, 0
bootdevice db 0