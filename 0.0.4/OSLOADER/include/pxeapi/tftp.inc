%ifndef __EXT_FAT_INC__INCLUDED__
%define __EXT_FAT_INC__INCLUDED__
%ifndef __TFTP_INC__INCLUDED__
%define __TFTP_INC__INCLUDED__

[bits 16]

; ds:si = filename
; edx = destination
ReadFile:
	pushad										; Save Registers
	push edx									; Save Destination
	push si										; puts(LOADMSG)
	mov si, LOADMSG
	call puts
	pop si
	call puts									; puts(FileName)
	mov DWORD [FileSize], 0						; Reset FileSize
	call TFTP_OPEN								; Open TFTP Connection
	.ReadLoop:
		mov si, WAITMSG							; puts(WAITMSG)
		call puts
		mov di, 0x1000							; TempBuffer = 0x1000
		call TFTP_READ_PACKET					; Read TFTP Packet
		movzx ecx, ax							; Copy PacketSize to ECX
		add DWORD [FileSize], ecx				; FileSize += PacketSize
		pop edx									; Restore Destination
		mov edi, edx							; Copy Destination
		push edx								; Save Destination
		movzx eax, WORD [PXENV_TFTP_READ_t.PacketNumber] ; PacketNumber
		dec ax									; PacketNumber --
		movzx edx, WORD [PXENV_TFTP_OPEN_t.PacketSize] ; PacketSize
		mul edx									; PacketNumber *= PacketSize
		add edi, eax							; Destination += PacketNumber
		push ecx								; Save PacketSize
		mov esi, 0x1000							; Source = 0x1000
		call memcpy								; Copy Source to Destination
		pop ecx									; Restore PacketSize
		cmp cx, WORD [PXENV_TFTP_OPEN_t.PacketSize] ; Is the packet the size we expected
		je .ReadLoop							; Yes, there are probably more so loop.
	call TFTP_CLOSE								; Close TFTP Connection
	pop edx										; Restore Destination
	mov si, DONEMSG								; puts(DONEMSG)
	call puts
	popad										; Restore Registers
	mov eax, DWORD [FileSize]					; FileSize returns in EAX
	ret											; Return

LOADMSG db "Loading TFTP File ", 0
FileSize dd 0

%endif
%endif
