[bits 32]

[global _start]                  ; LINKER CALLS START
[global start]
[global sys_stack]
[extern kmain]
[global BootPageDir]

KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
PageTabelVirtualBase equ 0xF0000000
PageTablePageNumber equ (PageTabelVirtualBase >> 20)
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 20)  ; Page directory index of kernels 4KB PTE.
            ; NOTE: devide by 1MB, as each entry is 4Bytes which makes 4MB Devide for page table
start equ (_start - KERNEL_VIRTUAL_BASE)

_start:                          ; Start point of KERNEL
    mov eax, (BootPageTbl - KERNEL_VIRTUAL_BASE)
    or eax, 3
    mov DWORD [BootPageDir - KERNEL_VIRTUAL_BASE], eax
    mov DWORD [BootPageDir - KERNEL_VIRTUAL_BASE + KERNEL_PAGE_NUMBER], eax
    
    mov eax, (PageDirTbl - KERNEL_VIRTUAL_BASE)
    or eax, 3
    mov DWORD [BootPageDir - KERNEL_VIRTUAL_BASE + PageTablePageNumber], eax
    
    mov ecx, BootPageTbl
    sub ecx, KERNEL_VIRTUAL_BASE
    mov edx, 0x3
    .pgtblloop:
        mov DWORD [ecx], edx
        add edx, 0x1000
        add ecx, 4
        cmp edx, 0x400003
        jne .pgtblloop
    
    mov ecx, PageDirTbl
    sub ecx, KERNEL_VIRTUAL_BASE
    mov edx, BootPageDir
    sub edx, KERNEL_VIRTUAL_BASE
    or edx, 3
    mov DWORD [ecx], edx
    add ecx, 4
    add edx, 0x1000
    mov DWORD [ecx], edx
    add ecx, KERNEL_PAGE_NUMBER
    ;add ecx, 0xBF8
    mov DWORD [ecx], edx
    add edx, 0x1000
    add ecx, 0x300
    mov DWORD [ecx], edx




    mov ecx, (BootPageDir - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx
    mov ecx, cr0
    or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
    mov cr0, ecx
    jmp startstub + KERNEL_VIRTUAL_BASE


align 0x1000
BootPageDir:
    times (1024) dd 0
BootPageTbl:
    times (1024) dd 0
PageDirTbl:
    times (1024) dd 0

 
startstub:
    mov esp, sys_stack          ; This points the stack to our new stack area
    push ebx                    ; Our Boot Infomation pointer is stored in ebx
    call kmain
    .done:
        cli
        hlt
        jmp .done

SECTION .bss
    resb 0x2000                   ; This reserves 8KBytes of memory here
sys_stack:
