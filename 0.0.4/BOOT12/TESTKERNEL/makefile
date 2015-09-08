OBJDIR=SRC
OBJECTS=$(addprefix $(OBJDIR)/,start.o main.o)
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
