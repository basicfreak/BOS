#include <i386/cpu/i686.h>

	
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

typedef struct gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed)) x86_GDT_ENT_u, *x86_GDT_ENT_p;

extern void GDTBase(void);
x86_GDT_ENT_p GDT = (x86_GDT_ENT_p) ((uint32_t)&GDTBase);

typedef struct idt_entry {
	uint16_t base_lo;
	uint16_t sel;
	uint8_t always0;
	uint8_t flags;
	uint16_t base_hi;
}__attribute__((packed)) x86_IDT_ENT_u, *x86_IDT_ENT_p;

extern void IDTBase(void);
x86_IDT_ENT_p IDT = (x86_IDT_ENT_p) ((uint32_t)&IDTBase);

extern void TSSBase(void);
extern void TSS_END(void);
uint32_t TSS = (uint32_t) &TSSBase;

void InstallGDTRing(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void InstallIDTGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void InstallAllGates(void);

void _i686_init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	_SSE_init();
	_FPU_init();
	uint32_t TSS_Size = (((uint32_t)&TSS_END) - TSS - 1);
// Kernel Mode Ring 0
	InstallGDTRing (1, 0, 0xFFFFFFFF,
		(_MEMORY | _CODEDATA | _EXEC_CODE | _READWRITE),
			(_4K | _32BIT)); //0x08

    InstallGDTRing (2, 0, 0xFFFFFFFF,
		(_MEMORY | _CODEDATA | _READWRITE),
			(_4K | _32BIT)); //0x10
			
// USER MODE RING 3
	InstallGDTRing (3, 0, 0xFFFFFFFF,
		(_READWRITE | _EXEC_CODE | _CODEDATA | _MEMORY | _DPL),
			(_4K | _32BIT)); //0x18
			
	InstallGDTRing (4, 0, 0xFFFFFFFF,
		(_READWRITE | _CODEDATA | _MEMORY | _DPL),
			(_4K | _32BIT)); //0x20

// TSS
	InstallGDTRing(5, TSS, TSS_Size, ( _ACCESS | _EXEC_CODE | _DPL | _MEMORY ), 0);
	_TSS_setESP();
	_GDT_Load();
	InstallAllGates();
	_IDT_Load();
	_PIC_init();
	_TSS_Flush();
}

void InstallGDTRing(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	GDT[num].base_low = (base & 0xFFFF);
	GDT[num].base_middle = (base >> 16) & 0xFF;
	GDT[num].base_high = (uint8_t) ((base >> 24) & 0xFF);
	GDT[num].limit_low = (limit & 0xFFFF);
	GDT[num].granularity = ((limit >> 16) & 0x0F);
	GDT[num].granularity = (uint8_t) (GDT[num].granularity | (gran & 0xF0));
	GDT[num].access = access;
}

void InstallIDTGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
/*
#ifdef DEBUG_FULL
	DEBUG_printf("(\"%s\":Lint %i) %s(0x%x, 0x%x, 0x%x, 0x%x);\n", __FILE__, (__LINE__ - 3),
			__func__, num, base, sel, flags);
#endif*/
	IDT[num].base_lo = (base & 0xFFFF);
	IDT[num].base_hi = (uint16_t) ((base >> 16) & 0xFFFF);
	IDT[num].sel = sel;
	IDT[num].always0 = 0;
	IDT[num].flags = flags;
	/*idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_hi = (uint16_t) ((base >> 16) & 0xFFFF);
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;*/
}

void InstallAllGates()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	InstallIDTGate((uint8_t) 0x0, (uint32_t) IDT0, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x1, (uint32_t) IDT1, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x2, (uint32_t) IDT2, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x3, (uint32_t) IDT3, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x4, (uint32_t) IDT4, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x5, (uint32_t) IDT5, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x6, (uint32_t) IDT6, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x7, (uint32_t) IDT7, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x8, (uint32_t) IDT8, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x9, (uint32_t) IDT9, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0xa, (uint32_t) IDT10, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0xb, (uint32_t) IDT11, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0xc, (uint32_t) IDT12, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0xd, (uint32_t) IDT13, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0xe, (uint32_t) IDT14, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0xf, (uint32_t) IDT15, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x10, (uint32_t) IDT16, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x11, (uint32_t) IDT17, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x12, (uint32_t) IDT18, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x13, (uint32_t) IDT19, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x14, (uint32_t) IDT20, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x15, (uint32_t) IDT21, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x16, (uint32_t) IDT22, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x17, (uint32_t) IDT23, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x18, (uint32_t) IDT24, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x19, (uint32_t) IDT25, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x1a, (uint32_t) IDT26, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x1b, (uint32_t) IDT27, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x1c, (uint32_t) IDT28, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x1d, (uint32_t) IDT29, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x1e, (uint32_t) IDT30, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x1f, (uint32_t) IDT31, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x20, (uint32_t) IDT32, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x21, (uint32_t) IDT33, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x22, (uint32_t) IDT34, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x23, (uint32_t) IDT35, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x24, (uint32_t) IDT36, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x25, (uint32_t) IDT37, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x26, (uint32_t) IDT38, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x27, (uint32_t) IDT39, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x28, (uint32_t) IDT40, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x29, (uint32_t) IDT41, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x2a, (uint32_t) IDT42, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x2b, (uint32_t) IDT43, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x2c, (uint32_t) IDT44, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x2d, (uint32_t) IDT45, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x2e, (uint32_t) IDT46, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x2f, (uint32_t) IDT47, (uint16_t)  (0x08), (uint8_t) (0xEE));
	InstallIDTGate((uint8_t) 0x30, (uint32_t) IDT48, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x31, (uint32_t) IDT49, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x32, (uint32_t) IDT50, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x33, (uint32_t) IDT51, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x34, (uint32_t) IDT52, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x35, (uint32_t) IDT53, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x36, (uint32_t) IDT54, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x37, (uint32_t) IDT55, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x38, (uint32_t) IDT56, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x39, (uint32_t) IDT57, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x3a, (uint32_t) IDT58, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x3b, (uint32_t) IDT59, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x3c, (uint32_t) IDT60, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x3d, (uint32_t) IDT61, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x3e, (uint32_t) IDT62, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x3f, (uint32_t) IDT63, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x40, (uint32_t) IDT64, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x41, (uint32_t) IDT65, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x42, (uint32_t) IDT66, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x43, (uint32_t) IDT67, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x44, (uint32_t) IDT68, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x45, (uint32_t) IDT69, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x46, (uint32_t) IDT70, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x47, (uint32_t) IDT71, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x48, (uint32_t) IDT72, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x49, (uint32_t) IDT73, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x4a, (uint32_t) IDT74, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x4b, (uint32_t) IDT75, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x4c, (uint32_t) IDT76, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x4d, (uint32_t) IDT77, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x4e, (uint32_t) IDT78, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x4f, (uint32_t) IDT79, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x50, (uint32_t) IDT80, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x51, (uint32_t) IDT81, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x52, (uint32_t) IDT82, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x53, (uint32_t) IDT83, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x54, (uint32_t) IDT84, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x55, (uint32_t) IDT85, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x56, (uint32_t) IDT86, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x57, (uint32_t) IDT87, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x58, (uint32_t) IDT88, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x59, (uint32_t) IDT89, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x5a, (uint32_t) IDT90, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x5b, (uint32_t) IDT91, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x5c, (uint32_t) IDT92, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x5d, (uint32_t) IDT93, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x5e, (uint32_t) IDT94, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x5f, (uint32_t) IDT95, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x60, (uint32_t) IDT96, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x61, (uint32_t) IDT97, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x62, (uint32_t) IDT98, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x63, (uint32_t) IDT99, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x64, (uint32_t) IDT100, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x65, (uint32_t) IDT101, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x66, (uint32_t) IDT102, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x67, (uint32_t) IDT103, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x68, (uint32_t) IDT104, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x69, (uint32_t) IDT105, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x6a, (uint32_t) IDT106, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x6b, (uint32_t) IDT107, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x6c, (uint32_t) IDT108, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x6d, (uint32_t) IDT109, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x6e, (uint32_t) IDT110, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x6f, (uint32_t) IDT111, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x70, (uint32_t) IDT112, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x71, (uint32_t) IDT113, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x72, (uint32_t) IDT114, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x73, (uint32_t) IDT115, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x74, (uint32_t) IDT116, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x75, (uint32_t) IDT117, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x76, (uint32_t) IDT118, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x77, (uint32_t) IDT119, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x78, (uint32_t) IDT120, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x79, (uint32_t) IDT121, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x7a, (uint32_t) IDT122, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x7b, (uint32_t) IDT123, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x7c, (uint32_t) IDT124, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x7d, (uint32_t) IDT125, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x7e, (uint32_t) IDT126, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x7f, (uint32_t) IDT127, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x80, (uint32_t) IDT128, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x81, (uint32_t) IDT129, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x82, (uint32_t) IDT130, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x83, (uint32_t) IDT131, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x84, (uint32_t) IDT132, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x85, (uint32_t) IDT133, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x86, (uint32_t) IDT134, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x87, (uint32_t) IDT135, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x88, (uint32_t) IDT136, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x89, (uint32_t) IDT137, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x8a, (uint32_t) IDT138, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x8b, (uint32_t) IDT139, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x8c, (uint32_t) IDT140, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x8d, (uint32_t) IDT141, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xEE, (uint32_t) IDT142, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x8f, (uint32_t) IDT143, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x90, (uint32_t) IDT144, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x91, (uint32_t) IDT145, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x92, (uint32_t) IDT146, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x93, (uint32_t) IDT147, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x94, (uint32_t) IDT148, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x95, (uint32_t) IDT149, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x96, (uint32_t) IDT150, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x97, (uint32_t) IDT151, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x98, (uint32_t) IDT152, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x99, (uint32_t) IDT153, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x9a, (uint32_t) IDT154, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x9b, (uint32_t) IDT155, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x9c, (uint32_t) IDT156, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x9d, (uint32_t) IDT157, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x9e, (uint32_t) IDT158, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0x9f, (uint32_t) IDT159, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa0, (uint32_t) IDT160, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa1, (uint32_t) IDT161, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa2, (uint32_t) IDT162, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa3, (uint32_t) IDT163, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa4, (uint32_t) IDT164, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa5, (uint32_t) IDT165, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa6, (uint32_t) IDT166, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa7, (uint32_t) IDT167, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa8, (uint32_t) IDT168, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xa9, (uint32_t) IDT169, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xaa, (uint32_t) IDT170, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xab, (uint32_t) IDT171, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xac, (uint32_t) IDT172, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xad, (uint32_t) IDT173, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xae, (uint32_t) IDT174, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xaf, (uint32_t) IDT175, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb0, (uint32_t) IDT176, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb1, (uint32_t) IDT177, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb2, (uint32_t) IDT178, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb3, (uint32_t) IDT179, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb4, (uint32_t) IDT180, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb5, (uint32_t) IDT181, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb6, (uint32_t) IDT182, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb7, (uint32_t) IDT183, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb8, (uint32_t) IDT184, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xb9, (uint32_t) IDT185, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xba, (uint32_t) IDT186, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xbb, (uint32_t) IDT187, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xbc, (uint32_t) IDT188, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xbd, (uint32_t) IDT189, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xbe, (uint32_t) IDT190, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xbf, (uint32_t) IDT191, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc0, (uint32_t) IDT192, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc1, (uint32_t) IDT193, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc2, (uint32_t) IDT194, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc3, (uint32_t) IDT195, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc4, (uint32_t) IDT196, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc5, (uint32_t) IDT197, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc6, (uint32_t) IDT198, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc7, (uint32_t) IDT199, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc8, (uint32_t) IDT200, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xc9, (uint32_t) IDT201, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xca, (uint32_t) IDT202, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xcb, (uint32_t) IDT203, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xcc, (uint32_t) IDT204, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xcd, (uint32_t) IDT205, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xce, (uint32_t) IDT206, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xcf, (uint32_t) IDT207, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd0, (uint32_t) IDT208, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd1, (uint32_t) IDT209, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd2, (uint32_t) IDT210, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd3, (uint32_t) IDT211, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd4, (uint32_t) IDT212, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd5, (uint32_t) IDT213, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd6, (uint32_t) IDT214, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd7, (uint32_t) IDT215, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd8, (uint32_t) IDT216, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xd9, (uint32_t) IDT217, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xda, (uint32_t) IDT218, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xdb, (uint32_t) IDT219, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xdc, (uint32_t) IDT220, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xdd, (uint32_t) IDT221, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xde, (uint32_t) IDT222, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xdf, (uint32_t) IDT223, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe0, (uint32_t) IDT224, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe1, (uint32_t) IDT225, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe2, (uint32_t) IDT226, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe3, (uint32_t) IDT227, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe4, (uint32_t) IDT228, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe5, (uint32_t) IDT229, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe6, (uint32_t) IDT230, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe7, (uint32_t) IDT231, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe8, (uint32_t) IDT232, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xe9, (uint32_t) IDT233, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xea, (uint32_t) IDT234, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xeb, (uint32_t) IDT235, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xec, (uint32_t) IDT236, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xed, (uint32_t) IDT237, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xee, (uint32_t) IDT238, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xef, (uint32_t) IDT239, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf0, (uint32_t) IDT240, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf1, (uint32_t) IDT241, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf2, (uint32_t) IDT242, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf3, (uint32_t) IDT243, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf4, (uint32_t) IDT244, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf5, (uint32_t) IDT245, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf6, (uint32_t) IDT246, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf7, (uint32_t) IDT247, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf8, (uint32_t) IDT248, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xf9, (uint32_t) IDT249, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xfa, (uint32_t) IDT250, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xfb, (uint32_t) IDT251, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xfc, (uint32_t) IDT252, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xfd, (uint32_t) IDT253, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xfe, (uint32_t) IDT254, (uint16_t)  (0x08), (uint8_t) (0xee));
	InstallIDTGate((uint8_t) 0xff, (uint32_t) IDT255, (uint16_t)  (0x08), (uint8_t) (0xee));
}