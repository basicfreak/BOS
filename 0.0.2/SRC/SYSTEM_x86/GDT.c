/*
./LIBSRC/GRT.C
*/

#include <GDT.h>

void InstallGDTRing(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt.Entry[num].base_low = (base & 0xFFFF);
	gdt.Entry[num].base_middle = (base >> 16) & 0xFF;
	gdt.Entry[num].base_high = (uint8_t) ((base >> 24) & 0xFF);
	gdt.Entry[num].limit_low = (limit & 0xFFFF);
	gdt.Entry[num].granularity = ((limit >> 16) & 0x0F);
	gdt.Entry[num].granularity = (uint8_t) (gdt.Entry[num].granularity | (gran & 0xF0));
	gdt.Entry[num].access = access;
}

void _GDT_init()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.2\t%s\tCompiled at %s on %s\tMAIN\t  Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
/* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * 10) - 1;
    gp.base = (uint32_t) &gdt;
// NULL
    InstallGDTRing (0, 0, 0, 0, 0); //0x00
	
// KERNEL MODE RING 0

    InstallGDTRing (1, 0, 0xFFFFFFFF,
			_MEMORY | _CODEDATA | _EXEC_CODE | _READWRITE,
			_4K | _32BIT); //0x08

    InstallGDTRing (2, 0, 0xFFFFFFFF,
			_MEMORY | _CODEDATA | _READWRITE,
			_4K | _32BIT); //0x10
			
// USER MODE RING 3
	InstallGDTRing (3, 0, 0xFFFFFFFF,
			_READWRITE | _EXEC_CODE | _CODEDATA | _MEMORY | _DPL,
			_4K | _32BIT); //0x18
			
	InstallGDTRing (4, 0, 0xFFFFFFFF,
			_READWRITE | _CODEDATA | _MEMORY | _DPL,
			_4K | _32BIT); //0x20

// Kernel Mode Ring 0 UNReal Mode
	InstallGDTRing (5, 0, 0xFFFFFFFF,
			_MEMORY | _CODEDATA | _EXEC_CODE | _READWRITE,
			_4K); //0x28

/* Flush out the old GDT and install the new changes! */
    FlushGDT ();
	
}