%ifndef __GDT_INC__INCLUDED__
%define __GDT_INC__INCLUDED__

[bits 16]

init_GDT:
	lgdt [gdtinfo]
	ret

;----------------------------------------------------
;                        GDT
;----------------------------------------------------
gdtinfo:
   dw (gdt_end - gdt - 1)   ;last byte in table
   dd (gdt)                ;start of table

gdt         dd 0,0        ; entry 0 is always unused
flatcode    db 0xFF, 0xFF, 0, 0, 0, 10011010b, 10001111b, 0
flatdata    db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0
pmflatcode  db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0
gdt_end:

%endif