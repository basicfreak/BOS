;----------------------------------------------------
;                   stdio.inc
;                   2014-08-14
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __STDIO_INC__INCLUDED__
%define __STDIO_INC__INCLUDED__

[bits 16]

;----------------------------------------------------
;                       puts
;         SI = String Pointer (0 Terminated)
;----------------------------------------------------
puts:
	pusha
	.PutsLoop:
		lodsb
		or al, al
		jz PutsDone ; Check for Terminator (0)
		mov ah, 0eh
		int 10h ; Print Charactor To Screen (INT 10h 0Eh)
		jmp .PutsLoop
	PutsDone:
		popa
		ret
		
;----------------------------------------------------
;                    ReadSectors
;                EBX = Starting LBA
;                EDX = Destination
;                CX = Sector Count
;----------------------------------------------------
ReadSectors:
	pushad
	mov DWORD [DAP.LBALow], EBX ; Setup DAP
	mov DWORD [DAP.dest], EDX
	mov WORD [DAP.count], cx
	mov dl, BYTE [bootDrive]
	mov si, DAP
	xor ax, ax
	mov ah, 0x42
	int 0x13 ; call int 13h 42h
	jc ERROR
	test ah, ah
	jnz ERROR
	popad
	ret

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


%endif	;__STDIO_INC__INCLUDED__