; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                                      PIC
;                          01/03/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This is only ment to remap the PIC to 0x80 - 0x8F and copy the IVT entries to
; the new INTs. And set / restore masks on CPU mode changes.

bits 16
section .text

global INIT_PIC
global PIC_ReMask

INIT_PIC:
	pusha								; Save registers
	cli									; IF = 0

	mov di, PrimaryMask					; Destination = PrimaryMask
	mov dx, 0x21						; Port = 0x21, PIC1 Data
	in al, dx							; Get PIC1 Masks
	stosb								; Save PIC1 Masks
	xor dl, 0x80						; Port = 0xA1, PIC2 Data
	in al, dx							; Get PIC2 Masks
	stosb								; Save PIC2 Masks

	xor dl, 0x81						; Port = 0x20, PIC1 Command
	mov al, 0x11						; Command = ICW1
	out dx, al							; Send ICW1 to PIC1
	xor dl, 0x80						; Port = 0xA0, PIC2 Command
	out dx, al							; Send ICW1 to PIC2

	xor dl, 0x81						; Port = 0x21, PIC1 Data
	mov al, 0x80						; Data = PIC1 INT Offset
	out dx, al							; Set PIC1 INT Offset
	xor dl, 0x80						; Port = 0xA1, PIC2 Data
	mov al, 0x88						; Data = PIC2 INT Offset
	out dx, al							; Set PIC2 INT Offset

	xor dl, 0x80						; Port = 0x21, PIC1 Data
	mov al, 0x04						; Data = Cascade IRQ
	out dx, al							; Send Cascade IRQ to PIC1
	xor dl, 0x80						; Port = 0xA1, PIC2 Data
	mov al, 0x02						; Data = Cascade Identity
	out dx, al							; Send Cascade ID to PIC2

	xor dl, 0x80						; Port = 0x21, PIC1 Data
	dec al								; Data = ICW4_8086
	out dx, al							; Send ICW4_8086 to PIC1
	xor dl, 0x80						; Port = 0xA1, PIC2 Data
	out dx, al							; Send ICW4_8086 to PIC2

	mov si, PrimaryMask					; Source = PrimaryMask
	xor dl, 0x80						; Port = 0x21, PIC1 Data
	lodsb								; Load PIC1 Masks
	out dx, al							; Set PIC1 Masks
	xor dl, 0x80						; Port = 0xA1, PIC2 Data
	lodsb								; Load PIC2 Masks
	out dx, al							; Set PIC2 Masks

	mov di, PrimaryMask					; Destination = PrimaryMask
	mov al, 0xFB						; PIC1 Mask for PM = All but IRQ2 Mask
	stosb								; Store PIC1 PMs Mask
	mov al, 0xFF						; PIC2 Mask for PM = All Masked
	stosb								; Store PIC2 PMs Mask

	mov si, 0x20						; Source = INT 8 Offset
	mov di, 0x200						; Destination = INT 80h Offset
	mov cx, 8							; Count = 8
	rep movsd							; Copy Source to Destination * Count DW
	mov si, 0x1C0						; Source = INT 70h Offset
	mov cx, 8							; Count = 8
	rep movsd							; Copy Source to Destination * Count DW

	sti									; IF = 1
	popa								; Restore registers
	ret									; Return to caller

PIC_ReMask:
	pusha								; Save registers
	mov dx, 0xA1						; Port = 0xA1, PIC2 Data
	in al, dx							; Get PIC2 Masks
	lock xchg BYTE [SecondaryMask], al	; Exchange with old Mask
	out dx, al							; Set PIC2 Masks
	xor dl, 0x80						; Port = 0x21, PIC1 Data
	in al, dx							; Get PIC1 Masks
	lock xchg BYTE [PrimaryMask], al	; Exchange with old Mask
	out dx, al							; Set PIC1 Masks
	popa								; Restore registers
	ret									; Return to caller

section .data
align 0x04

PrimaryMask db 0
SecondaryMask db 0
