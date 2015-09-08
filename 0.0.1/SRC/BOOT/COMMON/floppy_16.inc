;----------------------------------------------------
;                  floppy_16.inc
;                   2014-07-02
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __FLOPPY_16_INC__INCLUDED__
%define __FLOPPY_16_INC__INCLUDED__

[bits 16]

;----------------------------------------------------
;                       LBACHS
;             AX=>LBA Address to convert
;                      OUTPUTS:
;              aSector, aHead, aTrack
;----------------------------------------------------
LBACHS:
	xor dx, dx                           ; prepare dx:ax for operation
	div WORD [bpbSectorsPerTrack]        ; calculate
	inc dl                               ; adjust for sector 0
	mov BYTE [aSector], dl
	xor dx, dx                           ; prepare dx:ax for operation
	div WORD [bpbHeadsPerCylinder]       ; calculate
	mov BYTE [aHead], dl
	mov BYTE [aTrack], al
	ret

;----------------------------------------------------
;                   ReadSectors
;           CX=>Number of sectors to read
;              AX=>Starting sector
;           ES:BX=>Buffer to read to
;----------------------------------------------------
ReadSectors:
	.MAIN:
		mov di, 0x0005                   ; five retries for error
	.SECTORLOOP:
;----------------------------------------------------
;                   Save Registers
;----------------------------------------------------
		pusha
;----------------------------------------------------
;           Convert Starting Sector to CHS
;----------------------------------------------------
		call LBACHS
;----------------------------------------------------
;                   BIOS INT 13h
;----------------------------------------------------
		mov ah, 0x02                     ; read sector
		mov al, 0x01                     ; 1 Sector At A Time
		mov ch, BYTE [aTrack]            ; track
		mov cl, BYTE [aSector]           ; sector
		mov dh, BYTE [aHead]             ; head
		mov dl, BYTE [bsDriveNumber]     ; drive
		int 0x13                         ; INT 13h
		jnc .SUCCESS                     ; test for read error
		xor ax, ax                       ; BIOS reset disk
		int 0x13                         ; INT 13h
		dec di                           ; decrement error counter
;----------------------------------------------------
;                 Restore Registers
;----------------------------------------------------
		popa
;----------------------------------------------------
;            If Error Attempt Read Again
;----------------------------------------------------
		jnz .SECTORLOOP
		int 0x18
	.SUCCESS:
;----------------------------------------------------
;                 Restore Registers
;----------------------------------------------------
		popa
;----------------------------------------------------
;               Prepare For Next Sector
;----------------------------------------------------
		add bx, WORD [bpbBytesPerSector] ; buffer
		inc ax                           ; sector
;----------------------------------------------------
;                   Read Next Sector
;----------------------------------------------------
		loop .MAIN
;----------------------------------------------------
;                      Return
;----------------------------------------------------
	ret

;----------------------------------------------------
;                      VARIABLES
;----------------------------------------------------
datasector  dw 0x0000
cluster     dw 0x0000

aSector db 0x00
aHead   db 0x00
aTrack  db 0x00


%endif	;__FLOPPY_16_INC__INCLUDED__