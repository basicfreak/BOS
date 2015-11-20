%ifndef __STRING_INC__INCLUDED__
%define __STRING_INC__INCLUDED__

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
						db 0x66
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

%endif