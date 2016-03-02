; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                                      GDT
;                          03/01/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

section .GDTSection
global gdt

; Well this is my GDT:

; 0x08 = 16-bit Code Segment 0000-FFFF Ring 0
; 0x10 = 16-bit Data Segment 0000-FFFF Ring 0
; 0x18 = 32-bit Code Segment 00000000-FFFFFFFF Ring 0
; 0x20 = 32-bit Data Segment 00000000-FFFFFFFF Ring 0
; 0x2B = 32-bit Code Segment 00000000-FFFFFFFF Ring 3
; 0x23 = 32-bit Data Segment 00000000-FFFFFFFF Ring 3
; 0x28 = 64-bit Code Segment Ring 0
; 0x30 = 64-bit Data Segment Ring 0
; 0x3B = 64-bit Code Segment Ring 3
; 0x43 = 64-bit Data Segment Ring 3

gdt: ; this is at 0x9000
	dw (gdt.end - gdt - 1)   ;last byte in table
	dd (gdt)               ;start of table
	dw 0
	; .null		dq 0
	.16C		db 0xFF, 0xFF, 0, 0, 0, 10011000b, 00000000b, 0
	.16D		db 0xFF, 0xFF, 0, 0, 0, 10010010b, 00000000b, 0
	.32C0		db 0xFF, 0xFF, 0, 0, 0, 10011000b, 11001111b, 0
	.32D0		db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0
	.32C3		db 0xFF, 0xFF, 0, 0, 0, 11111000b, 11001111b, 0
	.32D3		db 0xFF, 0xFF, 0, 0, 0, 11110010b, 11001111b, 0
	.64C0		db 0xFF, 0xFF, 0, 0, 0, 10011000b, 10101111b, 0
	.64D0		db 0xFF, 0xFF, 0, 0, 0, 10010010b, 10101111b, 0
	.64C3		db 0xFF, 0xFF, 0, 0, 0, 11111000b, 10101111b, 0
	.64D3		db 0xFF, 0xFF, 0, 0, 0, 11110010b, 10101111b, 0
	.Empty		times 501 dq 0
.end: ; this is at 0xA000
