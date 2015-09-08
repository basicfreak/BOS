/*
./INCLUDE/BOSBOOT.H
*/
#ifndef				BOSBOOT_H_INCLUDED
	#define				BOSBOOT_H_INCLUDED

	#include <api.h>

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
		//VBE 2.0
		uint32_t PhysBasePtr;
		uint32_t OffScreenMemOffset;
		uint16_t OffScreenMemSize;
		//VBE 3.0
		uint16_t LinBytesPerScanLine;
		uint8_t BnkNumberOfImagePages;
		uint8_t LinNumberOfImagePages;
		uint8_t LinRedMaskSize;
		uint8_t LinRedFieldPosition;
		uint8_t LinGreenMaskSize;
		uint8_t LinGreenFieldPosition;
		uint8_t LinBlueMaskSize;
		uint8_t LinBlueFieldPosition;
		uint8_t LinRsvdMaskSize;
		uint8_t LinRsvedFieldPosition;
		uint8_t MaxPixelClock;
	}__attribute__((packed)) VideoInfo_t, *VideoInfo_p;

/*
Offset  Size    Description     (Table 00079)
00h    WORD    mode attributes (see #00080)
02h    BYTE    window attributes, window A (see #00081)
03h    BYTE    window attributes, window B (see #00081)
04h    WORD    window granularity in KB
06h    WORD    window size in KB
08h    WORD    start segment of window A (0000h if not supported)
0Ah    WORD    start segment of window B (0000h if not supported)
0Ch    DWORD   -> FAR window positioning function (equivalent to AX=4F05h)
10h    WORD    bytes per scan line
---remainder is optional for VESA modes in v1.0/1.1, needed for OEM modes---
12h    WORD    width in pixels (graphics) or characters (text)
14h    WORD    height in pixels (graphics) or characters (text)
16h    BYTE    width of character cell in pixels
17h    BYTE    height of character cell in pixels
18h    BYTE    number of memory planes
19h    BYTE    number of bits per pixel
1Ah    BYTE    number of banks
1Bh    BYTE    memory model type (see #00082)
1Ch    BYTE    size of bank in KB
1Dh    BYTE    number of image pages (less one) that will fit in video RAM
1Eh    BYTE    reserved (00h for VBE 1.0-2.0, 01h for VBE 3.0)
---VBE v1.2+ ---
1Fh    BYTE    red mask size
20h    BYTE    red field position
21h    BYTE    green mask size
22h    BYTE    green field size
23h    BYTE    blue mask size
24h    BYTE    blue field size
25h    BYTE    reserved mask size
26h    BYTE    reserved mask position
27h    BYTE    direct color mode info
*/

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

	extern BootInfo_t BOOTINF;

#endif