[bits 32]


[global start]                  ; LINKER CALLS START
[global sysstack]
[extern kmain]
[global KVirtBase]
[global BootPageDir]
[global PMMBase]
[global VMMBase]


start equ (_start - KVirtBase)
KernelPageDirectory equ (_KernelPageDirectory - KVirtBase)
BootPageDir equ (_KernelPageDirectory - KVirtBase)
KernelTableEntStart equ 0x380
VMMBase equ 0xFFBFF000
PMMBase equ 0xFFA00000
sysstack equ 0xFFBFDFFF
KVirtBase equ 0xFF000000
PhysMemStack equ (_PhysMemStack - KVirtBase)

_start:
; PRESERVE ebx!

    ; Create Page Directory Structure
    mov edi, KernelPageDirectory
    mov eax, KernelPageDirectory
    mov ecx, 0xE00
    or eax, 3
    mov DWORD [edi + 0xFFC], eax
    add eax, 0x7D000
    mov DWORD [edi], eax
    sub eax, 0x7C000
    .DIRLoop:
        mov DWORD [edi + ecx], eax
        add ecx, 4
        add eax, 0x1000
        cmp ecx, 0xE40
        jl .DIRLoop
    or eax, 4
    .APITblLoop:
        mov DWORD [edi + ecx], eax
        add ecx, 4
        add eax, 0x1000
        cmp ecx, 0xFF0
        jl .APITblLoop
    and eax, 0xFFFFF003
    .MoreKernelArea:
        mov DWORD [edi + ecx], eax
        add ecx, 4
        add eax, 0x1000
        cmp ecx, 0xFFC
        jl .MoreKernelArea

    ; Map BIOS (first 1MB) and Kernel Text (first 3MB)
    mov ecx, 0x400
    mov eax, 3
    mov edi, KernelPageDirectory
    add edi, 0x7D000
    .TextEntries:
        mov DWORD [edi], eax
        add edi, 4
        add eax, 0x1000
        dec ecx
        jnz .TextEntries
    mov ecx, 0x200
    mov eax, PhysMemStack
    or eax, 3
    mov edi, KernelPageDirectory
    add edi, 0x7F800
    .Stacks:
        mov DWORD [edi], eax
        add edi, 4
        add eax, 0x1000
        dec ecx
        jnz .Stacks

    ; Enable Paging
    mov ecx, KernelPageDirectory
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

_KernelPageDirectory:
    resb 0x00081000
