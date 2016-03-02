; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                             System Initialization
;                          17/02/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; This is mostly test code ATM

bits 16

section .text
global start

extern BS_ReadFile
extern FILE_IO
extern gdt
extern AP_Strap
extern Enable_A20

VAR:
	.bootDrive		equ -127			; BYTE
	.bootInfo		equ -126			; BitField
		; 13/42 | 13/02 | PXE | ? | TFTP | ? | ExtFS | FATFS

bootDrive			equ 0x7B81
bootInfo			equ 0x7B82

bits 16
	
start:
	mov ax, 0x0012
	int 0x10

	mov si, MSG.Identify
	call puts

	pushfd								;Save EFLAGS
    pushfd								;Store EFLAGS
    pop eax
    xor eax, 0x00200000					;Invert the ID bit in stored EFLAGS
    push eax
    popfd								;Load stored EFLAGS (with ID bit inverted)
    pushfd								;Store EFLAGS again (ID bit may or may not be inverted)
    pop eax								;eax = modified EFLAGS (ID bit may or may not be inverted)
    pop ebx
    push ebx
    xor eax,ebx							;eax = whichever bits were changed
    popfd								;Restore original EFLAGS
    test eax,0x00200000					;eax = zero if ID bit can't be changed, else non-zero
    jz .Fail
    
    mov si, MSG.Ok
    call puts
    mov si, MSG.ProcessorMode
    call puts

    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .PMode

    mov eax, 0x80000001
    cpuid
    bt edx, 29
    jnc .PMode

    .LMode:
    	mov si, MSG.Ok
    	call puts
    	mov BYTE [AMD64], 0xFF
    	jmp .Continue

    .PMode:

    .Continue:
    	mov si, MSG.GDT
    	call puts
		lgdt [gdt]
		mov si, MSG.Done
		call puts
		mov si, MSG.BootDevice
		call puts
		mov al, BYTE [bp + VAR.bootInfo]
		bt ax, 0
		jc .FatDisk
		bt ax, 1
		jc .ExtDisk
		bt ax, 3
		jc .TftpPxe
		jmp ERROR
		
	.FatDisk:
		clc
		mov di, 0xA000
		mov cx, 3
		mov si, Files.FATDisk
		call BS_ReadFile
		jc .Fail
		mov si, MSG.Done
		call puts
		jmp .EnableA20
	.ExtDisk:
		clc
		mov di, 0xA000
		mov si, Files.ExtDisk
		call BS_ReadFile
		jc .Fail
		mov si, MSG.Done
		call puts
		jmp .EnableA20
	.TftpPxe:
		clc
		mov di, 0xA000
		mov si, Files.TFTPPXE
		call BS_ReadFile
		jc .Fail
		mov si, MSG.Done
		call puts
	.EnableA20:
		mov si, MSG.A20
		call puts
		call Enable_A20
        jc .Fail
	.GoToPM:
		mov si, MSG.Done
		call puts
		mov si, MSG.PM32
		call puts
		mov al, 0
		mov edx, PM_Entry
		jmp AP_Strap ; 0x8800


    .Fail:
    	mov si, MSG.Fail
    	call puts
    	jmp ERROR.halt
ERROR:
	mov si, ERROR_MSG
	call puts
	.halt:
		hlt
		jmp .halt

bits 32
puts32:
	pusha
	mov al, 1
	mov edx, .RMEnt
	jmp AP_Strap ; 0x8800
bits 16
	.RMEnt:
		call puts
		mov al, 0
		mov edx, .PMEnt
		jmp AP_Strap ; 0x8800
bits 32
	.PMEnt:
		popa
		ret
bits 16
puts:
	pusha									; Save Registers
	lodsb
	mov bl, al
	.StrLoop:
		lodsb								; mov al, BYTE [ds:si]; inc si
		test al, al							; test al for 0
		jz .Done							; Check for Terminator (0)
		mov ah, 0Eh							; Print Char To Screen Function
		int 10h								; Call BIOS
		jmp .StrLoop						; Loop
	.Done:
		popa								; Restore Registers
		ret									; Return

bits 32
PM_Entry:
	mov si, MSG.Done
	call puts32
	cmp BYTE [AMD64], 0
	je .Build86

	mov si, MSG.PDIR
	call puts32

	mov eax, 0x201000
	mov edi, 0x200000
	or eax, 0x0B
	stosd
	add eax, 0x1000
	mov edi, 0x201000
	stosd
	mov ecx, 512
	mov edi, 0x202000
	mov eax, 0x8B
	.PSELoop:
		stosd
		add eax, 0x200000
		add edi, 4
		loop .PSELoop
	mov eax, 0x200000
	mov ecx, eax
	add ecx, 3
	mov DWORD [eax + 0xFF8], ecx

	mov si, MSG.Done
	call puts32
	mov si, MSG.LOADBUILD
	call puts32

	mov esi, Files.BUILD64
	xor eax, eax
	call FILE_IO
	jc .Fail
	mov edi, 0x100000
	mov eax, 1
	call FILE_IO
	jc .Fail
	mov si, MSG.Done
	call puts32
xchg bx, bx
	mov edx, 0x100000
	mov al, 3
	mov ebx, 0x200000
	jmp AP_Strap ; 0x8800

	.Build86:
		mov si, MSG.LOADBUILD
		call puts32
		mov esi, Files.BUILD86
		xor eax, eax
		call FILE_IO
		jc .Fail
		mov edi, 0x100000
		mov eax, 1
		call FILE_IO
		jc .Fail
		mov si, MSG.Done
		call puts32
xchg bx, bx
		jmp 0x100000

	.Fail:
		mov si, MSG.Fail
		call puts32
		mov edx, ERROR.halt
		mov al, 1
		jmp AP_Strap ; 0x8800

section .rodata

ERROR_MSG db 0xC, "ERROR: Could not initialize the system!", 0

MSG:
	.Identify		db 0x7, "Checking for CPUID            ", 0
	.ProcessorMode	db 0x7, "Checking for AMD64            ", 0
	.BootDevice		db 0x7, "Installing Boot Device Driver ", 0
	.GDT			db 0x7, "Installing GDT                ", 0
	.A20			db 0x7, "Enableing Address Line 20     ", 0
	.PM32			db 0x7, "Entering 32-bit Protected Mode", 0
	.PDIR			db 0x7, "Creating Temporary Page Dir   ", 0
	.LOADBUILD		db 0x7, "Loading BOS Builder           ", 0
	.Done			db 0x2, "                     [ DONE! ]", 0xA, 0xD, 0
	.Ok				db 0x2, "                     [  OK!  ]", 0xA, 0xD, 0
	.No				db 0xE, "                     [  NO!  ]", 0xA, 0xD, 0
	.Fail			db 0x4, "                     [ FAIL! ]", 0xA, 0xD, 0

Files:
	.FATDisk db "BOOT       ", "DRIVERS    ", "FATFS   BIN"
	.ExtDisk db "/boot/drivers/extdisk.bin", 0
	.TFTPPXE db "/boot/drivers/tftppxe.bin", 0
	.BUILD86 db "/boot/x86/build.bin", 0
	.BUILD64 db "/boot/x64/build.bin", 0

section .data

AMD64 db 0
