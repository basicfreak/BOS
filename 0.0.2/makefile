OBJDIR=SRC
OBJECTS=$(addprefix $(OBJDIR)/,START.o MAIN.o LIB/KLIB.o SYSTEM_x86/_ASM_GDT.o SYSTEM_x86/GDT.o \
	SYSTEM_x86/_ASM_IDT.o SYSTEM_x86/IDT.o SYSTEM_x86/8259.o SYSTEM_x86/PIT.o MEM/PHYS.o MEM/VIRTUAL.o \
	MEM/PAGEFAULT.o API/API.o API/modules.o ELF/ELF.o)
OUTPUT=BIN/KERNEL.ELF
CC=gcc
AS=nasm
LD=ld
INCLUDES=SRC/INCLUDE/
CCFLAGS=-O3 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings \
	-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline \
		-Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes -Wunsafe-loop-optimizations \
			-std=gnu99 -nostdinc -fno-builtin -fno-stack-protector -march=i586 -m32 -I $(INCLUDES)
LDFLAGS=-melf_i386 -Tlink.ld
ASFLAGS=-felf

all: $(OBJECTS) link

clean:
	@echo CLEANING UP...
	-@rm $(OBJECTS) $(OUTPUT)

link:
	@echo LINKING $(OUTPUT)...
	@$(LD) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS)

.s.o: %.o: %.s
	@echo COMPILING $<...
	@$(AS) $(ASFLAGS) $< -o $@

.c.o: %.o: %.c
	@echo COMPILING $<...
	@$(CC) $(CCFLAGS) -c -o $@ $<
