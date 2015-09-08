;----------------------------------------------------
;                     A20.inc
;                   2015-07-20
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __A20_INC__INCLUDED__
%define __A20_INC__INCLUDED__

[bits 16]

enable_A20:
        pusha
        cli
        call check_A20
        test ax, ax
        jnz .Enabled
        call A20_BIOS
        call check_A20
        test ax, ax
        jnz .Enabled
        call A20_Fast
        call check_A20
        test ax, ax
        jnz .Enabled
        call A20_KBRDCTRL
        test ax, ax
        jz ERROR
        .Enabled:
                sti
                popa
                ret

check_A20:
        pushf
        push es
        push ds
        xor ax, ax
        mov es, ax

        not ax ; ax = 0xFFFF
        mov ds, ax

        mov di, 0x0500
        mov si, 0x0510

        mov al, byte [es:di]
        push ax

        mov al, byte [ds:si]
        push ax

        mov byte [es:di], 0x00
        mov byte [ds:si], 0xFF

        cmp byte [es:di], 0xFF

        pop ax
        mov byte [ds:si], al

        pop ax
        mov byte [es:di], al

        mov ax, 0
        je .__exit

        mov ax, 1

        .__exit:
                pop ds
                pop es
                popf
                ret

A20_BIOS:
        mov ax, 0x2401
        int 0x15
        ret

A20_KBRDCTRL: 
        call    .a20wait
        mov     al,0xAD
        out     0x64,al
 
        call    .a20wait
        mov     al,0xD0
        out     0x64,al
 
        call    .a20wait2
        in      al,0x60
        push    eax
 
        call    .a20wait
        mov     al,0xD1
        out     0x64,al
 
        call    .a20wait
        pop     eax
        or      al,2
        out     0x60,al
 
        call    .a20wait
        mov     al,0xAE
        out     0x64,al
 
        call    .a20wait
        ret
        .a20wait:
                in      al,0x64
                test    al,2
                jnz     .a20wait
                ret
        .a20wait2:
                in      al,0x64
                test    al,1
                jz      .a20wait2
                ret

A20_Fast:
        in al, 0x92
        or al, 2
        out 0x92, al
        ret

%endif