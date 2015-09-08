; /CPU/GDT_ASM.s

[bits 32]

[global	FlushGDT]
[extern	gp]

FlushGDT:
    lgdt [gp]        						; Load the GDT with our '_gp' which is a special pointer
    mov ax, 0x10      						; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   						; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               						; Returns back to the C code!
