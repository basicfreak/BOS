/*
./LIBSRC/GRT.C
*/

#include "GDT.H"

void install_ring(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (uint8_t) ((base >> 24) & 0xFF);
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);
	gdt[num].granularity = (uint8_t) (gdt[num].granularity | (gran & 0xF0));
	gdt[num].access = access;
}

void _GDT_init()
{
/* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * 10) - 1;
    gp.base = (uint32_t) &gdt;
// NULL
    install_ring (0, 0, 0, 0, 0);
	
// KERNEL MODE RING 0

    install_ring (1, 0, 0xFFFFFFFF,
			_MEMORY | _CODEDATA | _EXEC_CODE | _READWRITE,
			_4K | _32BIT);

    install_ring (2, 0, 0xFFFFFFFF,
			_MEMORY | _CODEDATA | _READWRITE,
			_4K | _32BIT);
			
// USER MODE RING 3
	install_ring (3, 0, 0xFFFFFFFF,
			_READWRITE | _EXEC_CODE | _CODEDATA | _MEMORY | _DPL,
			_4K | _32BIT);
			
	install_ring (4, 0, 0xFFFFFFFF,
			_READWRITE | _CODEDATA | _MEMORY | _DPL,
			_4K | _32BIT);

/* Flush out the old GDT and install the new changes! */
    gdt_flush ();
	
}