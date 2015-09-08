[bits 32]


[global start]                  ; LINKER CALLS START
[global sysstack]
[extern kmain]
[global KVirtBase]
[global BootPageDir]
[global PMMBase]
[global VMMBase]

sysstack equ 0xFFBFDFFF
KVirtBase equ 0xFF000000
VMMBase equ 0xFFBFF000
PMMBase equ 0xFFB00000

start equ (_start - KVirtBase)

PageDir equ (_PageDIR - KVirtBase)
KPageTbl equ (_KPageTbl - KVirtBase)
SPageTbl equ (_SPageTbl - KVirtBase)
PageDirTbl equ (_PageDirTbl - KVirtBase)
PhysMemStack equ (_PhysMemStack - KVirtBase)

KPageTblNum equ (KVirtBase >> 20)
SPageTblNum equ (0xFF800000 >> 20)
PageDirTblNum equ (0xFFC00000 >> 20)

_start:                          ; Start point of KERNEL
    ; Set up Page DIR
    mov edx, PageDir
    mov ecx, KPageTbl
    or ecx, 0x00000003
    mov [edx], ecx
    mov [edx + (KPageTblNum)], ecx
    mov ecx, SPageTbl
    or ecx, 0x00000003
    mov [edx + (SPageTblNum)], ecx
    mov ecx, PageDir
    or ecx, 0x00000003
    mov [edx + (PageDirTblNum)], ecx

    ; Setup BIOS/OSLDR And KERNEL (3MB / 9MB)
    mov edx, KPageTbl
    mov ecx, 0x00000003
    .LoopKPageTbl:
        mov [edx], ecx
        add ecx, 0x00001000
        add edx, 0x00000004
        cmp ecx, 0x00401003
        jne .LoopKPageTbl

    ; Setup Stacks and first part of TLB
    mov edx, SPageTbl
    mov ecx, PhysMemStack
    or ecx, 0x00000003
    add edx, 0x00000800
    .LoopSPageTbl:
        mov [edx], ecx
        add ecx, 0x00001000
        add edx, 0x00000004
        cmp ecx, (PageDir + 0x00002000)
        jl .LoopSPageTbl
   
    ; Setup Rest of TLB
;    mov edx, PageDirTbl
;    mov ecx, KPageTbl
;    or ecx, 0x00000003
;    mov [edx + KPageTblNum], ecx
;    mov [edx], ecx
;    mov ecx, SPageTbl
;    or ecx, 0x00000003
;    mov [edx + SPageTblNum], ecx
;    mov ecx, PageDirTbl
;    or ecx, 0x00000003
;    mov [edx + PageDirTblNum], ecx

    ; Enable Paging
    mov ecx, PageDir
    mov cr3, ecx
    mov ecx, cr0
    or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
    mov cr0, ecx
    jmp (startstub + KVirtBase)


 
startstub:
    mov esp, sysstack          ; This points the stack to our new stack area
    push ebx                    ; Our Boot Infomation pointer is stored in ebx
    call kmain
    .done:
        cli
        hlt
        jmp .done







SECTION .bss
align 0x00001000

_PhysMemStack:
    resb 0x00100000

_sys_stack_Bottom:
    resb 0x000FF000                   ; This reserves .996MBytes of memory here
_sys_stack:

_PageDIR:
    resb 0x00001000

_KPageTbl:
    resb 0x00001000

_SPageTbl:
    resb 0x00001000

_PageDirTbl:
    resb 0x00001000