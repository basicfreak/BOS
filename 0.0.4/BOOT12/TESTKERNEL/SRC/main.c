typedef char bool;
typedef int size_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

#define NULL 0
#define TRUE 1
#define FALSE 0
#define true 1
#define false 0

typedef struct MMap_Str {
 
	uint64_t Base; // base address uint64_t
	uint64_t Length; // length uint64_t
	uint32_t Type; // entry Type
	uint32_t ACPI; // extended
 
}__attribute__((packed)) MMap_t, *MMap_p;

typedef struct ModList_Str {
	uint32_t ModStart;
	uint32_t ModEnd;
}__attribute__((packed)) ModL_t, *ModL_p;

typedef struct BOOTINFO_Str
{
	uint8_t BootDevice;
	MMap_p MMapAddr;
	uint16_t MMapEnts;
	uint32_t VBEInfo;
	uint32_t CurVBEInfo;
	uint32_t KernelStart;
	uint32_t KernelEnd;
	ModL_p ModList;
	uint8_t ModCount;
	uint32_t FrameBuffer;
}__attribute__((packed)) BootInfo_t, *BootInfo_p;

void kmain(BootInfo_p BINFO);

void kmain(BootInfo_p BINFO)
{
	unsigned short *VIDMEM = (unsigned short *) BINFO->FrameBuffer;
	// 800X600X16
	unsigned loc;
	for (loc = 0; loc < (800*600); loc++)
		VIDMEM[loc] = (unsigned short) 0x0F0F;
	for (;;)
		__asm__ __volatile__("hlt");
}