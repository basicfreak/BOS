; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                 ReadOnly Data
;                          01/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; If you need an explanation of what this is, then you are in the wrong place...

section .rodata

global MSG.Identify
global MSG.ProcessorMode
global MSG.BootDevice
global MSG.GDT
global MSG.A20
global MSG.PM32
global MSG.LM64
global MSG.PDIR
global MSG.PMM
global MSG.VMM
global MSG.Linker
global MSG.PIC
global MSG.MMAP
global MSG.Done
global MSG.Ok
global MSG.No
global MSG.Fail
global MSG.Error
global Files.PM_FAT
global Files.PM_EXT
global Files.PM_PXE
global Files.LM_FAT
global Files.LM_EXT
global Files.LM_PXE
global Files.PM_PMM
global Files.PM_VMM
global Files.PM_Link
global Files.LM_PMM
global Files.LM_VMM
global Files.LM_Link

MSG:
	.Identify		db 0x7, "Getting CPUID                 ", 0
	.ProcessorMode	db 0x7, "Checking for AMD64            ", 0
	.BootDevice		db 0x7, "Installing Boot Device Driver ", 0
	.GDT			db 0x7, "Installing GDT                ", 0
	.A20			db 0x7, "Enableing Address Line 20     ", 0
	.PM32			db 0x7, "Entering 32-bit Protected Mode", 0
	.LM64			db 0x7, "Entering 64-bit Long Mode     ", 0
	.PDIR			db 0x7, "Creating Temporary Page Dir   ", 0
	.PMM			db 0x7, "Loading Pysical Memory Manager", 0
	.VMM			db 0x7, "Loading Virtual Memory Manager", 0
	.Linker			db 0x7, "Loading BOS Linker / Builder  ", 0
	.PIC            db 0x7, "Remapping PIC                 ", 0
	.MMAP			db 0x7, "Reading Memory Map            ", 0
	.Done			db 0x2, "                     [ DONE! ]", 0xA, 0xD, 0
	.Ok				db 0x2, "                     [  OK!  ]", 0xA, 0xD, 0
	.No				db 0xE, "                     [  NO!  ]", 0xA, 0xD, 0
	.Fail			db 0x4, "                     [ FAIL! ]", 0xA, 0xD, 0
	.Error          db 0xC, 0xA, 0xD, "ERROR: Could not initialize the system!", 0

Files:
	.PM_FAT		db "BOOT       ", "X86        ", "FATFS   BIN"
	.LM_FAT		db "BOOT       ", "X64        ", "FATFS   BIN"
	.PM_EXT		db "/boot/x86/extdisk.bin", 0
	.LM_EXT		db "/boot/x64/extdisk.bin", 0
	.PM_PXE		db "/boot/x86/tftppxe.bin", 0
	.LM_PXE		db "/boot/x64/tftppxe.bin", 0
	.PM_PMM		db "/boot/x86/pmm.bin", 0
	.PM_VMM		db "/boot/x86/vmm.bin", 0
	.PM_Link	db "/boot/x86/build.bin", 0
	.LM_PMM		db "/boot/x64/pmm.bin", 0
	.LM_VMM		db "/boot/x64/vmm.bin", 0
	.LM_Link	db "/boot/x64/build.bin", 0
