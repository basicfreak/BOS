/*
./INCLUDE/BOSBOOT.H
*/
#ifndef				BOSBOOT_H_INCLUDED
	#define				BOSBOOT_H_INCLUDED

	#include <KLIB.h>

	typedef struct VideoModeInfo_Str {
		uint16_t attributes;
		uint8_t WinAAttri;
		uint8_t WinBAttri;
		uint16_t WinGranularity;
		uint16_t WinSize;
		uint16_t WinAStart;
		uint16_t WinBStart;
		uint32_t WindowPosFunct;
		uint16_t BytesPerLine;
		//VBE v1.0/1.1
		uint16_t Width;
		uint16_t Height;
		uint8_t CharWidth;
		uint8_t CharHeight;
		uint8_t Planes;
		uint8_t BitsPerPix;
		uint8_t Banks;
		uint8_t MemModel;
		uint8_t BankSize;
		uint8_t ImgPages;
		uint8_t RESERVED;
		//VBE v1.2+
		uint8_t RedMask;
		uint8_t RedField;
		uint8_t GreenMask;
		uint8_t GreenField;
		uint8_t BlueMask;
		uint8_t BlueField;
		uint8_t ReservedMaskSize;
		uint8_t ReservedMaskPos;
		uint8_t ColorModeInfo;
	}__attribute__((packed)) VideoInfo_t, *VideoInfo_p;

	typedef struct VideoPModeInterface {
		uint16_t SetWindowOffset;
		uint16_t SetDispStartOffset;
		uint16_t SetPrimPalOffset;
		uint16_t IOMEMTableOffset;
		uint8_t CODEDATA[0x1F8];
	}__attribute__((packed)) VidInt_t, *VidInt_p;

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
		VideoInfo_p CurVBEInfo;
		uint32_t KernelStart;
		uint32_t KernelEnd;
		ModL_p ModList;
		uint8_t ModCount;
		uint32_t FrameBuffer;
		uint32_t Mem_Low;
		uint32_t Mem_High;
		VidInt_p PModeINT;
	}__attribute__((packed)) BootInfo_t, *BootInfo_p;

#endif