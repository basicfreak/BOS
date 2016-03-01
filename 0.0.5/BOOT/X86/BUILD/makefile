OBJDIR=obj
SRCDIR=src
OUTPUT=../BUILD.BIN
OBJECTS=start.o

CC=gcc
AS=nasm
LD=ld
INCLUDES=./include
CCFLAGS=-O3 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings \
	-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline \
		-Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes -Wunsafe-loop-optimizations \
			-Wno-unused-parameter -std=gnu99 -fno-builtin -fno-stack-protector -ffreestanding -m32
LDFLAGS=-Tlink.ld
ASFLAGS=-felf32

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

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo -n "\tCompiling $<..."
	@$(CC) $(CCFLAGS) -c -o $@ $<
	@echo "DONE!"
