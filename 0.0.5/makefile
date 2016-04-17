# Define the Module's Output File(s) Here:
MODULES=boot/sysinit.bin boot/x86/fatfs.bin boot/x64/fatfs.bin boot/x86/pmm.bin\
		boot/x64/pmm.bin boot/x86/vmm.bin boot/x64/vmm.bin boot/x86/build.bin  \
		boot/x64/build.bin boot/x86/idt.elf boot/x64/idt.elf

HDDIMG=hdd.img
HDDOFFSET=32256
HDDMNT=hdd_mount

INCDIR=include/
CCMACHINE=-mno-mmx -mno-sse -mno-3dnow -mno-red-zone
CCWARNINGS=-Wall -Wextra -Wfatal-errors -Wpedantic -pedantic-errors -Wshadow   \
	-Wunsafe-loop-optimizations -Wconversion -Wmissing-prototypes -Winline     \
		-Wnested-externs
CCCODE=-O3 -std=gnu99 -fno-stack-protector -fno-asynchronous-unwind-tables     \
	-fno-common -fno-builtin -ffreestanding

# These are all passed to the sub-makefiles
# I'm not using export as I don't want to mess up anything using these names...
# And I think it looks better this way ;) And it's my script ;)
CC=x86_64-elf-gcc
CC32=i686-elf-gcc
AS=nasm
LD=x86_64-elf-ld
OC=x86_64-elf-objcopy
CAT=cat
CP=cp
RM=rm
ECHO=echo
CCFLAGS=$(CCWARNINGS) $(CCMACHINE) $(CCCODE) -I $(INCDIR)
LDFLAGS=-Tlink.ld
ASFLAGS=-felf64 -i$(INCDIR)
AS32FLAGS=-felf32 -i$(INCDIR)
OCFLAGS=-O elf64-x86-64

# These are programs used by only this script
BOCHS=bochs
SUDO=sudo
MOUNT=mount
UMOUNT=umount
SYNC=sync
PATHS=$(basename $(MODULES))


all: build install run

build:
	@$(ECHO) "BUILD:"
	-@for MODULE in $(PATHS); do \
		$(ECHO) "    $$MODULE:" ; \
		$(MAKE) CC=$(CC) CC32=$(CC32) AS=$(AS) LD=$(LD) OC=$(OC) ECHO=$(ECHO)  \
			CCFLAGS="$(CCFLAGS)" LDFLAGS=$(LDFLAGS) ASFLAGS="$(ASFLAGS)"       \
			OCFLAGS="$(OCFLAGS)" AS32FLAGS="$(AS32FLAGS)" -C $$MODULE -s ; \
	done
	@$(ECHO) "DONE!"

clean:
	@$(ECHO) "CLEAN:"
	-@for MODULE in $(PATHS); do \
		$(ECHO) "    $$MODULE:" ; \
		$(MAKE) ECHO=$(ECHO) RM=$(RM) -C $$MODULE -s clean; \
	done
	@$(ECHO) "    $(HDDIMG):"
	@$(SUDO) $(MOUNT) -ooffset=$(HDDOFFSET) $(HDDIMG) $(HDDMNT)
	-@for MODULE in $(MODULES); do \
		$(ECHO) -n "        " ;\
		$(SUDO) $(RM) -fv $(HDDMNT)/$$MODULE ;\
	done
	@$(SYNC)
	@$(SUDO) $(UMOUNT) $(HDDMNT)
	@$(ECHO) "DONE!"

install:
	@$(ECHO) "INSTALL:"
	@$(ECHO) "    $(HDDIMG):"
	@$(SUDO) $(MOUNT) -ooffset=$(HDDOFFSET) $(HDDIMG) $(HDDMNT)
	-@for MODULE in $(MODULES); do \
		$(ECHO) -n "        " ;\
		$(SUDO) $(CP) -v --parents $$MODULE $(HDDMNT)/ ; \
	done
	@$(SYNC)
	@$(SUDO) $(UMOUNT) $(HDDMNT)
	@$(ECHO) "DONE!"

run:
	@$(ECHO) "RUN:"
	@$(BOCHS) -q
	@$(ECHO) "DONE!"
