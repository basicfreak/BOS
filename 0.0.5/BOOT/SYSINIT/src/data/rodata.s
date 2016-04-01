; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                 ReadOnly Data
;                          01/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

section .rodata

global MSG.Identify
global MSG.ProcessorMode
global MSG.BootDevice
global MSG.GDT
global MSG.A20
global MSG.PM32
global MSG.PDIR
global MSG.LOADBUILD
global MSG.PIC
global MSG.MMAP
global MSG.Done
global MSG.Ok
global MSG.No
global MSG.Fail
global MSG.Error
global Files.FATDisk
global Files.ExtDisk
global Files.TFTPPXE
global Files.BUILD86
global Files.BUILD64

MSG:
	.Identify		db 0x7, "Getting CPUID                 ", 0
	.ProcessorMode	db 0x7, "Checking for AMD64            ", 0
	.BootDevice		db 0x7, "Installing Boot Device Driver ", 0
	.GDT			db 0x7, "Installing GDT                ", 0
	.A20			db 0x7, "Enableing Address Line 20     ", 0
	.PM32			db 0x7, "Entering 32-bit Protected Mode", 0
	.PDIR			db 0x7, "Creating Temporary Page Dir   ", 0
	.LOADBUILD		db 0x7, "Loading BOS Builder           ", 0
	.PIC            db 0x7, "Remapping PIC                 ", 0
	.MMAP			db 0x7, "Initilizing Memory Map        ", 0
	.Done			db 0x2, "                     [ DONE! ]", 0xA, 0xD, 0
	.Ok				db 0x2, "                     [  OK!  ]", 0xA, 0xD, 0
	.No				db 0xE, "                     [  NO!  ]", 0xA, 0xD, 0
	.Fail			db 0x4, "                     [ FAIL! ]", 0xA, 0xD, 0
	.Error          db 0xC, 0xA, 0xD, "ERROR: Could not initialize the system!", 0

Files:
	.FATDisk db "BOOT       ", "DRIVERS    ", "FATFS   BIN"
	.ExtDisk db "/boot/drivers/extdisk.bin", 0
	.TFTPPXE db "/boot/drivers/tftppxe.bin", 0
	.BUILD86 db "/boot/x86/build.bin", 0
	.BUILD64 db "/boot/x64/build.bin", 0
