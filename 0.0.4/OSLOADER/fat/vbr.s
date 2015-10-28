[bits 16]
[org 0x7C00]

jmp start

bootDrive db 0
times (3-($-$$)) nop

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

start:
	; From MBR all segments are set, the stack is setup, and INTs are on!
	; dl = bootDrive
	; ds:si = Partition Table Entry
	mov BYTE [bootDrive], dl
	mov eax, DWORD [si + 8]
	mov DWORD [PartitionOffset], eax
	mov si, DoneMSG
	call puts
	mov si, IMAGENAME
	call FAT_FindFile_Root
	jc ERROR
	mov di, 0x7E00
	call FAT_LoadFile
	jc ERROR
	push DWORD [PartitionOffset]
	mov dl, BYTE [bootDrive]
	jmp 0x0000:0x7E00

ERROR:
	mov si, ERRMSG
	call puts
	.halt:
		hlt
		jmp .halt

%include "./include/stdio/basic_stdio.inc"
%include "./include/stdio/hdd.inc"
%include "./include/fat/basic_fat.inc"


DoneMSG db "DONE!", 0x0A, 0x0D, "Loading "
IMAGENAME db "OSLOADERSYS" ; Sounds so much better than stagetwo.bin
db "...", 0 ; See what I did there ;)
ERRMSG db "ERROR", 0

PartitionOffset dd 0

times (0x1FE - ($-$$)) nop
dw 0xAA55
