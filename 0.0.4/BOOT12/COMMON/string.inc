;----------------------------------------------------
;                   string.inc
;                   2015-07-09
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]

;----------------------------------------------------
;                     Text2Hex
;             INPUT: EDX - Text (ASCII)
;                 OUTPUT: DX = Hex
;----------------------------------------------------
Text2Hex:
	push ax
	push bx

	mov bx, 4
	mov ax, 0
	.DigitLoop:
		cmp dl, 'a'
		jl .NotCap
		sub dl, ('a' - 10)
		jmp .contLoop
		.NotCap:
			cmp dl, 'A'
			jl .NotLetter
			sub dl, ('A' - 10)
			jmp .contLoop
				.NotLetter:
					sub dl, '0'
					.contLoop:
						or al, dl
						db 0x66, 0x67
							shr edx, 8
						dec bx
						test bx, bx
						jz .DigitLoopDone
						shl ax, 4
						jmp .DigitLoop
	.DigitLoopDone:
		mov dx, ax
		pop bx
		pop ax
	ret

;----------------------------------------------------
;                     PrintHex
;                    INPUT:  AX
;                  OUTPUT: Screen
;----------------------------------------------------
PrintHex:
	pusha
	mov si, HexStrBuff
	add si, 5
	mov bx, 4
	.HexLoop:
		mov dl, al
		and dl, 0x0F
		cmp dl, 0xA
		jge .HexLetter
		add dl, '0'
		jmp .HexLooping
		.HexLetter:
			add dl, ('A' - 0xA)
		.HexLooping:
			mov [ds:si], dl
			dec bx
			test bx, bx
			jz .HexDone
			dec si
			shr ax, 4
			jmp .HexLoop
	.HexDone:
		dec si
		dec si
		call puts
	popa
	ret

HexStrBuff db "0x0000", 0