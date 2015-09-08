/*
./DRIVERSRC/SYSTEM/CPU/IDT.H
*/

#ifndef				IDT_H_INCLUDED
	#define				IDT_H_INCLUDED
	
	#include <FORMATTING.H>
	
	struct idt_entry
	{
		uint16_t base_lo;
		uint16_t sel;
		uint8_t always0;
		uint8_t flags;
		uint16_t base_hi;
	} __attribute__((packed));

	struct idt_ptr
	{
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed));

	// We have 256 IDT entries
	struct idt_entry idt[256];
	struct idt_ptr idtp;

	// Still in START.ASM...
	extern void idt_load(void);	
	
	void install_Gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
	void _IDT_init(void);
	
#endif
