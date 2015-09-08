OBJECTS=KERNEL/START.o KERNEL/MAIN.o SYSTEM/API/API.o SYSTEM/API/THREADMAN.o SYSTEM/CPU/8259.o \
  SYSTEM/CPU/DISASM.o SYSTEM/CPU/GDT.o SYSTEM/CPU/IDT.o SYSTEM/CPU/INT.o SYSTEM/CPU/IRQ.o SYSTEM/CPU/ISR.o \
  SYSTEM/CPU/TSS.o SYSTEM/FS/VFS.o SYSTEM/FS/FAT.o SYSTEM/MEM/PAGEFAULT.o SYSTEM/MEM/PHYSICAL.o SYSTEM/MEM/VIRTUAL.o \
  KERNEL/COMMAND.o HARDWARE/8042/8042.o HARDWARE/8042/KEYBOARD.o HARDWARE/8042/MOUSE.o \
  HARDWARE/BIOS.o HARDWARE/CMOS.o HARDWARE/RS232.o HARDWARE/FDC.o HARDWARE/IDE.o HARDWARE/LPT.o HARDWARE/PCI.o \
  HARDWARE/DMA.o HARDWARE/TIMER.o LIB/MATH.o LIB/STDIO.o LIB/STRING.o SYSTEM/DEVICE/DISKMAN.o SYSTEM/DEVICE/MBR.o
CC=gcc
AS=nasm
LD=ld
INCLUDES=INCLUDE/
CFLAGS=-m32 -O0 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes \
  -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wconversion \
  -Wstrict-prototypes -std=gnu99 -nostdinc -fno-builtin -fno-stack-protector -I $(INCLUDES)
LDFLAGS=-m elf_i386 -Tlink.ld
ASFLAGS=-f elf32

all: $(OBJECTS) link

clean:
	@echo CLEANING UP...
	-@rm $(OBJECTS) KERNEL.ELF

link:
	@echo LINKING KERNEL.ELF...
	@$(LD) $(LDFLAGS) -o KERNEL.ELF $(OBJECTS)

.s.o: %.o: %.s
	@echo COMPILING $<...
	@$(AS) $(ASFLAGS) $< -o $@

.c.o: %.o: %.c
	@echo COMPILING $<...
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
	