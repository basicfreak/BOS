/*
./SRC/MAIN.C
*/

#include <BOSBOOT.h>
#include <KLIB.h>
#include <GDT.h>
#include <IDT.h>
#include <8259.h>
#include <PIT.h>
#include <PHYSMEM.H>
#include <PAGEFAULT.H>
#include <VIRTUALMEM.H>
#include <API.h>
#include <modules.h>
#include <ELF.h>

void kmain(BootInfo_p BOOTINF);

//Entry Point on C Code.
void kmain(BootInfo_p BOOTINF)
{
	if(BOOTINF->KernelStart != 0x100000) {
#ifdef DEBUG_FULL
		DEBUG_print("ERROR with BOOT INFO\nBoot Failed.\n");
#endif
		return;
	}
	_GDT_init();
#ifdef DEBUG_FULL
	DEBUG_print("GDT Installed\n");
#endif
	_IDT_init();
#ifdef DEBUG_FULL
	DEBUG_print("IDT Installed\n");
#endif
	_8259_INIT();
#ifdef DEBUG_FULL
	DEBUG_print("8259 PIC Installed\n");
#endif
	_PIT_init();
#ifdef DEBUG_FULL
	DEBUG_print("PIT Installed\n");
#endif
	PHYSMEM_init(BOOTINF);
	_pageFault_init();
	VIRTUALMEM_init();

	sti();


/*
		uint32_t vbe_control_info;
		uint32_t vbe_mode_info;
		uint16_t vbe_mode;
		uint16_t vbe_interface_seg;
		uint16_t vbe_interface_off;
		uint16_t vbe_interface_len;
		uint64_t framebuffer_addr;
		uint32_t framebuffer_pitch;
		uint32_t framebuffer_width;
		uint32_t framebuffer_height;
		uint8_t framebuffer_bpp;
		uint8_t framebuffer_type;
*/
	_API_init();

	_API_Add_Symbol("BOOTINF", (uint32_t)BOOTINF);
#ifdef DEBUG_FULL
	DEBUG_print("API Installed\n");
#endif
#ifdef DEBUG_FULL
	DEBUG_printf("End of Kernel Data = 0x%x\n", (uint32_t) end);
	DEBUG_printf("Bottom of Kernel Stack = 0x%x\n", (uint32_t) sys_stack);
#endif



	/*DEBUG_print("Multiboot Module Testing:\n");
	DEBUG_printf("\tModule Count = %i\n", mbd->mods_count);
	DEBUG_printf("\tModule Table Base Address = 0x%x\n", mbd->mods_addr);
	multiboot_module_t *ModList2;
	for(uint32_t mods = 1; mods <= mbd->mods_count; mods++){
		ModList2 = (multiboot_module_t *) (mbd->mods_addr + ((mods - 1) * 16));
		DEBUG_printf("Module found at 0x%x to 0x%x, size %i Bytes\tCommand Line 0x%x\n", ModList2->mod_start, ModList2->mod_end, (ModList2->mod_end - ModList2->mod_start), ModList2->cmdline);
		//cli();
		ELF_Display_Data((void *) ModList2->mod_start);
		//sti();
	}
*/
#ifdef DEBUG_FULL
	DEBUG_print("Loading Boot Time Modules...");
#endif
	/*multiboot_module_t *ModList;
	for(uint32_t mods = 0; mods < mbd->mods_count; mods++){
		ModList = (multiboot_module_t *) (mbd->mods_addr + (mods * 16));
		Load_Mod((void *) ModList->mod_start);
	}*/
	ModL_p ModList = BOOTINF->ModList;
	for (uint8_t mod = 0; mod < BOOTINF->ModCount; mod++)
		Load_Mod((void *) ModList[mod].ModStart);
#ifdef DEBUG_FULL
	DEBUG_print("DONE!");
#endif
	for(;;)
		hlt();
}