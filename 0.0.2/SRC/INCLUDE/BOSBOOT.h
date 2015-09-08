/*
./INCLUDE/BOSBOOT.H
*/
#ifndef				BOSBOOT_H_INCLUDED
	#define				BOSBOOT_H_INCLUDED

	#include <KLIB.h>

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
		uint32_t Mem_Low;
		uint32_t Mem_High;
	}__attribute__((packed)) BootInfo_t, *BootInfo_p;

#endif