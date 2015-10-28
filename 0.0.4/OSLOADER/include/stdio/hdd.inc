%ifndef __HDD_INC__INCLUDED__
%define __HDD_INC__INCLUDED__

[bits 16]

;----------------------------------------------------
;                    ReadSectors
;                EBX = Starting LBA
;                EDX = Destination
;                CX = Sector Count
;----------------------------------------------------
ReadSectors:
	pushad							; Save Registers
	mov DWORD [DAP.LBALow], EBX		; Setup DAP
	mov DWORD [DAP.dest], EDX
	mov WORD [DAP.count], cx
	mov dl, BYTE [bootDrive]		; Set Boot Drive
	mov si, DAP						; Load DAP in SI
	mov ax, 0x4200					; Extended Read Function
	int 0x13
	jc ERROR						; Error
	test ah, ah
	jnz ERROR						; Error
	popad							; Restore Registers
	ret								; Return

;----------------------------------------------------
;            DATA SEGMENT FOR ReadSectors
;----------------------------------------------------
DAP:
	db 0x10
	db 0
	.count: dw 0
	.dest: dd 0
	.LBALow: dd 0
	.LBAHigh: dd 0

%endif
