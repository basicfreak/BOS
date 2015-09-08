/*
./LIBSRC/IRT.C
*/

#include "IDT.H"
#include <STDIO.H>

void install_Gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_hi = (uint16_t) ((base >> 16) & 0xFFFF);
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}


void _IDT_init()
{
	idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
	idtp.base = (uint32_t) &idt;
	// Clear IDT
	memset (&idt, 0, sizeof(struct idt_entry) * 256);
	idt_load();
}