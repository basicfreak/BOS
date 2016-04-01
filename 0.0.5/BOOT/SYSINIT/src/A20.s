; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                                      A20
;                          01/03/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 16
section .text

global Enable_A20

; Ok, a little explanation of why I did not do this in the same order as stated
; on the OSDev WiKi:

; I have found the A20_Fast method to be the most reliable, and have never seen
; this method #TF.

; On the other hand, the A20_BIOS method may fail gracefully or in a #TF

; And the 8042 "PS/2 Controller" does not exist in modern systems, some lock up,
; some #TF, and I do not time-out while waiting on the 8042. (even though I can
; easily add a time-out I fell it would not be worth the wasted bytes - as it is
; the last attempt before we error out - and this would also require a-lot of
; conditional jumps e.g. jc's throughout the A20_KBRDCTRL function)

Enable_A20:
	pusha								; Save registers
	cli									; IF = 0
	call check_A20						; Check if A20 is enabled
	jnc .Return							; Return if enabled.
	call A20_Fast						; Try A20 Fast method
	call check_A20						; Check if A20 is enabled
	jnc .Return							; Return if enabled
	call A20_BIOS						; Try A20 BIOS method
	call check_A20						; Check if A20 is enabled
	jnc .Return							; Return if enabled
	call A20_KBRDCTRL					; Try A20 KB Controller method
	jnc .Return							; Return if enabled
	stc									; Set carry flag we failed to enable A20
	.Return:
		sti								; IF = 1
		popa							; Restore registers
		ret								; Return to caller


check_A20:
	pushf								; Push CPU Flags
	push es								; Push Extra Segment
	push ds								; Push Data Segment

	xor ax, ax
	mov es, ax							; Extra Segment = 0
	not ax
	mov ds, ax							; Data Segment = 0xFFFF
	mov di, 0x0500						; Destination = 00000:0500h
	mov si, 0x0510						; Source = FFFF:0510h

	mov al, byte [es:di]				; Save Value of es:di
	push ax								; Push it to stack
	lodsb								; Save Value of ds:si
	push ax								; Push it to stack

	mov byte [es:di], 0x00				; Save 0x00 into es:di
	mov byte [ds:si], 0xFF				; Save 0xFF into ds:si
	cmp byte [es:di], 0xFF				; Does es:di = ds:si?

	pop ax								; Pop old ds:si Value
	mov byte [ds:si], al				; Restore ds:si to original Value
	pop ax								; Pop old es:di Value
	stosb 								; Restore es:di to original Value

	pop ds								; Pop Data Segment
	pop es								; Pop Extra Segment

	jne .Pass							; If es:di != ds:si We have A20
	popf								; Pop CPU Flags
	stc									; Set carry flag
	jmp .Return							; Skip next 2 instruction
	.Pass:
		popf							; Pop CPU Flags
		clc								; Clear carry flag
	.Return:
		ret								; Return to caller


A20_BIOS:
	mov ax, 0x2401						; BIOS Function 15/2401h
	int 0x15
	ret									; Return


A20_KBRDCTRL:
	mov dx, 0x64						; 8042 Command Port
	call .InputWait_8042				; Wait until 8042 is ready for input
	mov al, 0xAD
	out dx, al							; Disable First PS/2 Port
	call .InputWait_8042				; Wait until 8042 is ready for input
	mov al, 0xD0
	out dx, al							; Read Controller Output Port Command
	call .OutputWait_8042				; Wait until 8042 is ready to output
	in al, dx							; Read From Controller Output Port
	push ax								; Store Controller Output
	call .InputWait_8042				; Wait until 8042 is ready for input
	mov al, 0xD1
	out dx, al							; Write Controller Output Port Command
	call .InputWait_8042				; Wait until 8042 is ready for input
	pop ax								; Restore Controller Output
	or al, 2							; Set A20 Enable Bit
	out dx, al							; Write To Controller Output Port
	call .InputWait_8042				; Wait until 8042 is ready for input
	mov al, 0xAE
	out dx, al							; Enable First PS/2 Port
	call .InputWait_8042				; Wait until 8042 is ready for input
	ret									; Return to caller

	.InputWait_8042:
		in al, dx						; Read Controller Status Byte
		test al, 2						; Is it ready for input?
		jnz .InputWait_8042				; If not loop.
		ret								; Return

	.OutputWait_8042:
		in al, dx						; Read Controller Status Byte
		test al, 1						; Is it ready to output?
		jz .OutputWait_8042				; If not loop.
		ret								; Return


A20_Fast:
	in al, 0x92							; Read Byte from Port 0x92
	or al, 2							; Set A20 Enable Flag
	out 0x92, al						; Write Byte to Port 0x92
	ret									; Return to caller
