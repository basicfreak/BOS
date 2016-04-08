OBJDIR=obj
SRCDIR=src
INCDIR=./include/

OUTPUT=../VMM.BIN
OBJECTS=init.o

CC=x86_64-elf-gcc
AS=nasm
LD=x86_64-elf-ld
INCLUDES=./include
CC32FLAGS=-O3 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings \
	-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline \
		-Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes -Wunsafe-loop-optimizations \
			-Wno-unused-parameter -std=gnu99 -fno-builtin -fno-stack-protector -ffreestanding \
				-fno-asynchronous-unwind-tables -march=i686 -m32 -x c -I $(INCDIR)
CCFLAGS=-O3 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings \
	-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline \
		-Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes -Wunsafe-loop-optimizations \
			-Wno-unused-parameter -std=gnu99 -fno-builtin -fno-stack-protector -ffreestanding \
				-fno-asynchronous-unwind-tables -m64 -I $(INCDIR)
LDFLAGS=-Tlink.ld
ASFLAGS=-felf64 -i$(INCDIR)

all: $(addprefix $(OBJDIR)/,$(OBJECTS)) link

clean:
	@echo -n "\tCleaning up..."
	-@rm $(addprefix $(OBJDIR)/,$(OBJECTS)) $(OUTPUT)
	@echo "DONE!"

link:
	@echo -n "\tLinking $(OUTPUT)..."
	@$(LD) $(LDFLAGS) -o $(OUTPUT) $(addprefix $(OBJDIR)/,$(OBJECTS))
	@echo "DONE!"

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@echo -n "\tAssembling $<..."
	@$(AS) $(ASFLAGS) $< -o $@
	@echo "DONE!"

$(OBJDIR)/%.o: $(SRCDIR)/%.c32
	@echo -n "\tCompiling $<..."
	@$(CC) $(CC32FLAGS) -c -o temp.o $<
	@objcopy -O elf64-x86-64 temp.o $@
	@rm temp.o
	@echo "DONE!"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo -n "\tCompiling $<..."
	@$(CC) $(CCFLAGS) -c -o $@ $<
	@echo "DONE!"
