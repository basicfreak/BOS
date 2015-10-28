%ifndef __MEMIO_INC__INCLUDED__
%define __MEMIO_INC__INCLUDED__

[bits 16]

;----------------------------------------------------
;                       memset
;   INPUT: EDI - destination ECX - count AL - value
;----------------------------------------------------
memset:
	cli							; We CANNOT be interrupted here
	push es						; Save Real Mode Data Segments
	mov  ebx, cr0				; Fetch CR0
	push ebx					; Save CR0 for later
	or bl,1						; set pmode bit
	mov  cr0, ebx				; Write CR0
	mov bx, 10h					; set 32-BIT Data GDT ent
	mov es, bx
	jmp 8:.pmode				; set 16-BIT Code GDT ent
	.pmode:
		a32	rep stosb			; Set data to al in its 32-bit address
		pop ebx					; Get Old CR0 from stack
		mov  cr0, ebx			; Write CR0
		jmp 0x0:.done			; Jump to Real Mode Code Segment
	.done:
		pop es					; Restore Real Mode Data Segments
		sti						; Ok we can be interrupted again.
	ret

;----------------------------------------------------
;                       memcpy
;  INPUT: ESI - source EDI - destination ECX - count
;   NOTE: This counts in BYTES (1 Bytes per count)
;----------------------------------------------------
memcpy:
xchg bx, bx
	cli							; We CANNOT be interrupted here
	push ds						; Save Real Mode Data Segments
	push es
	mov  ebx, cr0				; Fetch CR0
	push ebx					; Save CR0 for later
	or bl,1						; set pmode bit
	mov  cr0, ebx				; Write CR0
	mov bx, 10h					; set 32-BIT Data GDT ent
	mov ds, bx
	mov es, bx
	jmp 8:.pmode				; set 16-BIT Code GDT ent
	.pmode:
		a32	rep	movsb			; copy data to its 32-bit address
		pop ebx					; Get Old CR0 from stack
		mov  cr0, ebx			; Write CR0
		jmp 0x0:.done			; Jump to Real Mode Code Segment
	.done:
		pop es					; Restore Real Mode Data Segments
		pop ds
		sti						; Ok we can be interrupted again.
	ret


%endif
