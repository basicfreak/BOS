OBJDIR=SRC
OBJECTS=$(addprefix $(OBJDIR)/,start.o kmain.o LIB/KLIB.o i386/cpu/gdt.o i386/cpu/_asm_gdt.o \
	i386/cpu/8259.o i386/cpu/idt.o i386/cpu/_asm_idt.o i386/cpu/tss.o i386/cpu/_asm_tss.o \
	i386/cpu/sse.o \
	i386/memory/physman.o i386/memory/virtman.o i386/memory/_asm_mem.o i386/memory/heapman.o \
	api/api.o api/elf.o api/modules.o LIB/hal.o LIB/vfs.o i386/hardware/pit/pit.o)
OUTPUT=BIN/KERNEL.ELF
CC=gcc
AS=nasm
LD=ld
INCLUDES=SRC/INCLUDE/
CCFLAGS=-O3 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings \
	-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline \
		-Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes -Wunsafe-loop-optimizations \
			-std=gnu99 -nostdinc -fno-builtin -fno-stack-protector -march=i686 -m32 -I $(INCLUDES)
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
