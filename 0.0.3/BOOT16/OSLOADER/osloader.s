;----------------------------------------------------
;                    OSLOADER.S
;                  (FAT16 Loader)
;     Bootsector Version 0.0.3.1 for BOS 0.0.3
;                   2015-08-16
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]
[org 0]
setup:
	cli
	mov ax, 0xbe0
	mov es, ax

	add si, 8
	mov ebx, DWORD [ds:si]
	mov cx, ds

	;xor ax, ax
	mov ds, ax
	mov fs, ax
	mov gs, ax

	mov BYTE [bootDrive], dl
	mov DWORD [PTOffset], ebx
	mov WORD [PTDS], cx
	sub si, 8
	mov WORD [PTSI], si
	sti
;----------------------------------------------------
;        Get Important BPB info from stack
;----------------------------------------------------
	pop DWORD [bpbHiddenSectors]
	pop WORD [bpbSectorsPerFAT]
	pop WORD [bpbRootEntries]
	pop WORD [bpbReservedSectors]
	pop ax
	mov BYTE [bpbSectorsPerCluster], al
;----------------------------------------------------
;                  Check Variables
;----------------------------------------------------
	mov ax, WORD [bpbReservedSectors]
	cmp ax, 0
	je ERROR
	mov ax, WORD [bpbSectorsPerFAT]
	cmp ax, 0
	je ERROR
	mov ax, WORD [bpbRootEntries]
	cmp ax, 0
	je ERROR
	mov al, BYTE [bpbSectorsPerCluster]
	cmp al, 0
	je ERROR
	
	
start:
	mov si, DONEMSG
	call puts

	mov si, A20MGS
	call puts

	call enable_A20

	mov si, DONEMSG
	call puts

	mov si, GDTMSG
	call puts


	cli
	push ds                ; save real mode
	mov  eax, cr0          ; switch to pmode by
	or al,1                ; set pmode bit
	mov  cr0, eax
	lgdt [gdtinfo]         ; load gdt register
	mov bx,10h
	mov ds,bx
	jmp 8:(pmode + 0xBE00)
[bits 32]
	pmode:
		push eax
		
		pop eax
		and al,0xFE            ; back to realmode
		mov  cr0, eax          ; by toggling bit again
		jmp 0x0:(huge_unreal + 0xBE00)
[bits 16]
	huge_unreal:
		pop ds                 ; get back old segment
		sti

	mov si, DONEMSG
	call puts

;----------------------------------------------------
;               Actual Stage Two Code
;----------------------------------------------------
	mov si, MEMMSG
	call puts

	call memmap
	mov di, 0x2E00
	mov al, BYTE [bootDrive]
	mov BYTE [es:di], al
	mov al, 0
	mov BYTE [es:di + 27], al
	mov eax, DWORD 0xE400
	mov DWORD [es:di + 23], eax

	mov si, DONEMSG
	call puts

	mov si, MSizeMGG
	call puts

	call MemSize

	mov si, DONEMSG
	call puts

	call ReadConfig
 ; If we return we error here
ERROR:
	mov si, ERRMSG
	call puts
	.hltlop:
		hlt
		jmp .hltlop

%include "./COMMON/stdio.inc"
%include "./COMMON/a20.inc"
%include "./COMMON/memio.inc"
%include "./COMMON/fat16.inc"
%include "./COMMON/fat16ext.inc"
%include "./COMMON/config.inc"
%include "./COMMON/string.inc"
%include "./COMMON/svga.inc"
%include "./COMMON/elf.inc"

;----------------------------------------------------
;                     Variables
;----------------------------------------------------
bpbSectorsPerCluster db 0
bpbReservedSectors DW 0
bpbRootEntries DW 0
bpbSectorsPerFAT DW 0
bpbHiddenSectors DD 0
bootDrive db 0
PTOffset dd 0
PTDS dw 0
PTSI dw 0

DONEMSG db "DONE!", 0x0A, 0x0D, 0
A20MGS db "Enabling Line A20...", 0
GDTMSG db "Setting up GDT...", 0
MEMMSG db "Getting Memory Map From BIOS...", 0
MSizeMGG db "Getting Total Memory From BIOS...", 0
ERRMSG db "ERROR!", 0


BootDir DB "BOOT       "
BootConf DB "BOOT    CFG"

;----------------------------------------------------
;                        GDT
;----------------------------------------------------
	gdtinfo:
	   dw (gdt_end - gdt - 1)   ;last byte in table
	   dd (gdt + 0xBE00)                ;start of table

	gdt         dd 0,0        ; entry 0 is always unused
	flatcode    db 0xFF, 0xFF, 0, 0, 0, 10011010b, 10001111b, 0
	flatdata    db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0
	pmflatcode    db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0
	gdt_end:
	
;times (0x1000 - ($-$$)) db 0;