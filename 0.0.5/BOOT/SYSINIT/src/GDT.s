section .GDTSection
global gdt

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
