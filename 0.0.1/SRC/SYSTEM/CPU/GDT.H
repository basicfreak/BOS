/*
./INCLUDE/GDT.H
*/

#ifndef				GDT_H_INCLUDED
	#define				GDT_H_INCLUDED
	
	#include <FORMATTING.H>
	
	enum I86_GDT_DESC {
		_ACCESS=0x01,			//00000001
		_READWRITE=0x02,		//00000010
		_EXPANSION=0x04,		//00000100
		_EXEC_CODE=0x08,		//00001000
		_CODEDATA=0x10,			//00010000
		_DPL=0x60,				//01100000
		_MEMORY=0x80			//10000000
	};
	enum I86_GDT_GRAND {
		_LIMITHI_MASK=0x0f,		//00001111
		_OS=0x10,				//00010000
		_32BIT=0x40,			//01000000
		_4K=0x80				//10000000
	};
//_4K | _32BIT | _LIMITHI_MASK)
	
	struct gdt_entry
	{
		uint16_t limit_low;
		uint16_t base_low;
		uint8_t base_middle;
		uint8_t access;
		uint8_t granularity;
		uint8_t base_high;
	} __attribute__((packed));

	struct gdt_ptr
	{
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed));

	struct gdt_entry gdt[10];
	struct gdt_ptr gp;

	extern void gdt_flush(void);
	
	void install_ring(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
	void _GDT_init(void);
	
#endif
