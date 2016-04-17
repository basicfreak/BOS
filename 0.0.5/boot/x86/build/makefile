OUTPUT=../build.bin
OBJECTS=start.o

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
	@$(AS) $(AS32FLAGS) $< -o $@
	@$(ECHO) "DONE!"

$(OBJDIR)%.o: $(SRCDIR)%.c
	@$(ECHO) -n "        Compiling $<..."
	@$(CC32) $(CCFLAGS) -c -o $@ $<
	@$(ECHO) "DONE!"
