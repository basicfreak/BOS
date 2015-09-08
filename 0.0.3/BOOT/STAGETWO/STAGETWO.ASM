;----------------------------------------------------
;                  STAGETWO.ASM
;                 (FAT12 FLOPPY)
;          Bootsector Version 0.0.2.1
;                   2015-07-04
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]
[org 0xBE00]
mov ax, 0x0BE0
mov es, ax
xor ax, ax
mov ds, ax
mov fs, ax
mov gs, ax
mov [bootdevice], dl
;----------------------------------------------------
;        Get Important BPB info from stack
;----------------------------------------------------
	mov si, BOOTMSG
	call puts
	pop ax
	mov BYTE [bsExtBootSignature], al
	pop ax
	mov BYTE [bsDriveNumber], al
	pop DWORD [bpbTotalSectorsBig]
	pop DWORD [bpbHiddenSectors]
	pop WORD [bpbHeadsPerCylinder]
	pop WORD [bpbSectorsPerTrack]
	pop WORD [bpbSectorsPerFAT]
	pop ax
	mov BYTE [bpbMedia], al
	pop WORD [bpbTotalSectors]
	pop WORD [bpbRootEntries]
	pop ax
	mov BYTE [bpbNumberOfFATs], al
	pop WORD [bpbReservedSectors]
	pop ax
	mov BYTE [bpbSectorsPerCluster], al
	pop WORD [bpbBytesPerSector]

;----------------------------------------------------
;                    Get UnReal
;----------------------------------------------------

call enable_A20
	cli
	push ds                ; save real mode
	mov  eax, cr0          ; switch to pmode by
	or al,1                ; set pmode bit
	mov  cr0, eax
	lgdt [gdtinfo]         ; load gdt register
	mov bx,10h
	mov ds,bx
	jmp 8:pmode
[bits 32]
	pmode:
		push eax
		
		pop eax
		and al,0xFE            ; back to realmode
		mov  cr0, eax          ; by toggling bit again
		jmp 0x0:huge_unreal
[bits 16]
	huge_unreal:
		pop ds                 ; get back old segment
		sti

;----------------------------------------------------
;                  Check Variables
;----------------------------------------------------
	mov ax, [bpbBytesPerSector]
	cmp ax, 0
	je ERROR_STUB
	mov al, [bpbSectorsPerCluster]
	cmp al, 0
	je ERROR_STUB
	mov al, [bpbNumberOfFATs]
	cmp al, 0
	je ERROR_STUB
	mov ax, [bpbSectorsPerFAT]
	cmp ax, 0
	je ERROR_STUB
	mov ax, [bpbSectorsPerTrack]
	cmp ax, 0
	je ERROR_STUB
	mov ax, [bpbHeadsPerCylinder]
	cmp ax, 0
	je ERROR_STUB

;----------------------------------------------------
;               Actual Stage Two Code
;----------------------------------------------------
	call memmap
	mov di, 0x2E00
	mov al, BYTE [bootdevice]
	mov BYTE [es:di], al
	mov al, 0
	mov BYTE [es:di + 27], al
	mov eax, DWORD 0xE400
	mov DWORD [es:di + 23], eax

	call MemSize

	call ReadConfig

;----------------------------------------------------
;                     ERROR STUB
;----------------------------------------------------
	ERROR_STUB:
		mov si, ERRORMSG
		call puts
		;cli
	.halt:
		hlt
		jmp .halt
;----------------------------------------------------
;                 Drivers / Includes
;----------------------------------------------------
%include "./COMMON/stdio_16.inc"
%include "./COMMON/floppy_16.inc"
%include "./COMMON/fat12Ext.inc"
%include "./COMMON/memio.inc"
%include "./COMMON/A20.inc"
%include "./COMMON/string.inc"
%include "./COMMON/VBESVGA.inc"
%include "./COMMON/config.inc"
%include "./COMMON/elf.inc"
;----------------------------------------------------
;                     Variables
;----------------------------------------------------
	bpbBytesPerSector	  	DW 0
	bpbSectorsPerCluster 	DB 0
	bpbReservedSectors		DW 0
	bpbNumberOfFATs			DB 0
	bpbRootEntries			DW 0
	bpbTotalSectors			DW 0
	bpbMedia				DB 0
	bpbSectorsPerFAT		DW 0
	bpbSectorsPerTrack		DW 0
	bpbHeadsPerCylinder		DW 0
	bpbHiddenSectors		DD 0
	bpbTotalSectorsBig		DD 0
	bsDriveNumber			DB 0
	bsExtBootSignature		DB 0
	bootdevice				DB 0

	BootDir					DB "BOOT       "
	BootConf				DB "BOOT    CFG"
	ERRORMSG				DB "ERROR", 0
	BOOTMSG db ".DONE!", 0x0A, 0x0D, 0
;----------------------------------------------------
;                        GDT
;----------------------------------------------------
	gdtinfo:
	   dw gdt_end - gdt - 1   ;last byte in table
	   dd gdt                 ;start of table

	gdt         dd 0,0        ; entry 0 is always unused
	flatcode    db 0xFF, 0xFF, 0, 0, 0, 10011010b, 10001111b, 0
	flatdata    db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0
	pmflatcode    db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0
	gdt_end:
	