# Define Output file and object files here:
OUTPUT=../SYSINIT.BIN
OBJECTS=main.o AP_Strap.o GDT.o A20.o PIC.o MMAP.o CPUID.o term.o data/rodata.o \
	init/common.o init/x86.o init/x86_64.o


OBJDIR=obj/
SRCDIR=src/

all: $(addprefix $(OBJDIR),$(OBJECTS)) link

clean:
	-@for File in $(addprefix $(OBJDIR),$(OBJECTS)) $(OUTPUT); do \
		$(ECHO) -n "        " ;\
		$(RM) -vf $$File ;\
	done

link:
	@$(ECHO) -n "        Linking $(OUTPUT)..."
	@$(LD) $(LDFLAGS) -o $(OUTPUT) $(addprefix $(OBJDIR),$(OBJECTS))
	@$(ECHO) "DONE!"

$(OBJDIR)%.o: $(SRCDIR)%.s
	@$(ECHO) -n "        Assembling $<..."
	@$(AS) $(ASFLAGS) $< -o $@
	@$(ECHO) "DONE!"

$(OBJDIR)%.o: $(SRCDIR)%.c32
	@$(ECHO) -n "        Compiling $<..."
	@$(CC) $(CC32FLAGS) -c -o temp.o $<
	@$(OC) $(OCFLAGS) temp.o $@
	@$(RM) temp.o
	@$(ECHO) "DONE!"

$(OBJDIR)%.o: $(SRCDIR)%.c
	@$(ECHO) -n "        Compiling $<..."
	@$(CC) $(CCFLAGS) -c -o $@ $<
	@$(ECHO) "DONE!"
