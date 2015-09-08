// ./SRC/i386/memory/physical.c

#include <i386/memory/physman.h>

#define _PMM_MAXENT 64

uint32_t PhysMemLocation = 0;
uint32_t TotalRam = 0;
uint32_t UsedRam = 0;
uint32_t FreeRam = 0;

typedef struct PMM_Entry {
	uint32_t Base;
	uint32_t Length;
} __attribute__((packed)) PMME_t, *PMME_p;

typedef struct PMM_Stack {
	PMME_t Entry[64];
} __attribute__((packed)) PMMS_t, *PMMS_p;

PMMS_p PMMStack;

void _PMM_update(void);
void _PMM_defrag(void);

void _PMM_init(BootInfo_p BOOTINF)
{
	ModL_p ModList = BOOTINF->ModList;
	if(BOOTINF->ModCount) PhysMemLocation = (ModList[(BOOTINF->ModCount - 1)].ModEnd + 1);
	else PhysMemLocation = ((uint32_t)end - 0xC0000000);
	uint32_t LengthOfKernel = ((PhysMemLocation + 0x1000) / 0x1000);
	if ((PhysMemLocation + 0x1000) % 0x1000)
		LengthOfKernel++;
	PMMStack = (PMMS_p) (PhysMemLocation + 0xC0000000);		//The Memory Table is at the very next page after end of kernel and modules.
	memset(PMMStack, 0, sizeof(PMMS_t));
	TotalRam = ((BOOTINF->Mem_Low + BOOTINF->Mem_High) / 1024);
	FreeRam = 0;
	UsedRam = TotalRam;
	MMap_p mmap;
	for(uint32_t z = 0; z < (BOOTINF->MMapEnts); z++) {
		mmap = (MMap_p) ((uint32_t)BOOTINF->MMapAddr + (z * 24));
		if(mmap->Type == 1) {
			//Just incase the address and lengnth are not page aligned
			//Also increase Addr and decrease Len or we will exceed usable ram area
			uint64_t AlignedAddr = (((mmap->Base%0x1000) < 0x1000) ? (mmap->Base+(0x1000-(mmap->Base%0x1000))) : mmap->Base);
			uint64_t AlignedLen = (mmap->Length-(mmap->Length%0x1000));
			if((mmap->Base%0x1000) < 0x1000) AlignedLen = AlignedLen - 1;
			uint32_t PageAddr = (uint32_t) (AlignedAddr / 0x1000);
			uint32_t PageLen = (uint32_t) (AlignedLen / 0x1000);

			if(PageAddr < 0x10) {
				uint32_t PageOff = 0x10 - PageAddr;
				PageAddr += PageOff;
				PageLen = PageLen - PageOff;
			} else if(PageAddr > 0xFF && PageAddr < LengthOfKernel) {
				uint32_t PageOff = LengthOfKernel - PageAddr;
				PageAddr += PageOff;
				PageLen = PageLen - PageOff;
			}

			_PMM_free((PageAddr << 12), PageLen);
		}
	}
}

void _PMM_free(uint32_t Addr, uint32_t Len)
{
	for(int e = 0; e < _PMM_MAXENT; e++)
		if(PMMStack->Entry[e].Length == 0) {
			PMMStack->Entry[e].Base = (Addr >> 12);
			PMMStack->Entry[e].Length = Len;
			// Need to Defrag ...
			_PMM_defrag();
			_PMM_update();
			return;
		}
}

void *_PMM_malloc(uint32_t Pages)
{
	for (int e = 0; e < _PMM_MAXENT; e++)
		if(PMMStack->Entry[e].Length >= Pages) {
			//Do stuff
			uint32_t TempBase = PMMStack->Entry[e].Base;
			PMMStack->Entry[e].Base += Pages;
			PMMStack->Entry[e].Length -= Pages;
			_PMM_update();
			return (void*) (TempBase * 0x1000);
		}
	return 0; // returns 0 on error (remember the first 64KB are Reserved By Kernel)
}

void _PMM_defrag()
{
	for (int x = 0; x < 2; x++) // Do this twice to make sure we get most of them.
		for (int e = 0; e < _PMM_MAXENT; e++)
			if(PMMStack->Entry[e].Length)
				for (int t = 0; t < _PMM_MAXENT; t++)
					if((PMMStack->Entry[t].Base + PMMStack->Entry[t].Length) == PMMStack->Entry[e].Base) {
						//Murge Entries
						PMMStack->Entry[e].Base = PMMStack->Entry[t].Base;
						PMMStack->Entry[e].Length += PMMStack->Entry[t].Length;
						PMMStack->Entry[t].Base = 0;
						PMMStack->Entry[t].Length = 0;
					}
}

void _PMM_update()
{
	uint32_t FreePages = 0;
	for (int e = 0; e < _PMM_MAXENT; e++)
		if(PMMStack->Entry[e].Length)
			FreePages += PMMStack->Entry[e].Length;
	FreeRam = (FreePages * 4);
	UsedRam = TotalRam - FreeRam;
#ifdef DEBUG
	DEBUG_printf("\n\tTotal Ram %i KB\n\tFree  Ram %i KB\n\tUsed  Ram %i KB\n", TotalRam, FreeRam, UsedRam);
#endif
}
