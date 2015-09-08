/*
./INCLUDE/GDT.H
*/

#ifndef				GDT_H_INCLUDED
	#define				GDT_H_INCLUDED
	
	#include <KLIB.h>
	
	enum x86_GDT_DESC {
		_ACCESS=0x01,			//00000001
		_READWRITE=0x02,		//00000010
		_EXPANSION=0x04,		//00000100
		_EXEC_CODE=0x08,		//00001000
		_CODEDATA=0x10,			//00010000
		_DPL=0x60,				//01100000
		_MEMORY=0x80			//10000000
	};
	enum x86_GDT_GRAND {
		_LIMITHI_MASK=0x0f,		//00001111
		_OS=0x10,				//00010000
		_32BIT=0x40,			//01000000
		_4K=0x80				//10000000
	};
//_4K | _32BIT | _LIMITHI_MASK)
	
	typedef struct gdt_entry
	{
		uint16_t limit_low;
		uint16_t base_low;
		uint8_t base_middle;
		uint8_t access;
		uint8_t granularity;
		uint8_t base_high;
	} __attribute__((packed)) x86_GDT_ENT_u, *x86_GDT_ENT_p;

	typedef struct gdt_ptr
	{
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed)) x86_GDT_PTR_u, *x86_GDT_PTR_p;

	typedef struct gdt_table
	{
		x86_GDT_ENT_u	Entry[10];
	} __attribute__((packed)) x86_GDT_TBL_u, *x86_GDT_TBL_p;

	x86_GDT_TBL_u gdt;
	x86_GDT_PTR_u gp;

	extern void FlushGDT(void);
	
	void InstallGDTRing(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
	void _GDT_init(void);
	
#endif
