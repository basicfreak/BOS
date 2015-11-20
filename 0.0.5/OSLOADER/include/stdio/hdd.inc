%ifndef __HDD_INC__INCLUDED__
%define __HDD_INC__INCLUDED__

[bits 16]
DAP equ 0x0800
;----------------------------------------------------
;                    ReadSectors
;                EBX = Starting LBA
;                EDX = Destination
;                CX = Sector Count
;----------------------------------------------------
ReadSectors:
	pusha                         ; Save Registers
	mov si, DAP
	mov WORD [si], 0x000C
	mov DWORD [si + 8], ebx       ; Setup Disk Address Packet, DAP LBA Low
	mov WORD [si + 6], 0
	mov WORD [si + 4], di         ; DAP Destination Buffer
	mov WORD [si + 2], cx         ; DAP Sector Count
	mov dl, BYTE [bp - 13]        ; bootDrive
	mov ax, 0x4200                ; Extended Read Function
	int 0x13                      ; Call BIOS
	jc ERROR                      ; if carry flag is set Error
	popa                          ; Restore Registers
	ret

%endif
