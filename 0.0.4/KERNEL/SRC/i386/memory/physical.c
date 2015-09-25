#include <BOSBOOT.h>
#include <i386/memory/physical.h>

PMMENT_p PMM = (PMMENT_p) ((uint32_t)&PMMBase);

uint32_t PMM_Entries;

void _PMM_defrag(void);

void _PMM_init(BootInfo_p BOOTINF)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
	DEBUG_printf("PMM = 0x%x\n", PMM);
#endif
	uint32_t EndOfKernelData = 0;
	ModL_p ModList = BOOTINF->ModList;
	if(BOOTINF->ModCount) EndOfKernelData = (ModList[(BOOTINF->ModCount - 1)].ModEnd + 1);
	else EndOfKernelData = ((uint32_t)end - (uint32_t)KVirtBase);

	if (EndOfKernelData % PAGESIZE)
		EndOfKernelData += (PAGESIZE - (EndOfKernelData % PAGESIZE));

	TotalRam = (BOOTINF->Mem_Low + BOOTINF->Mem_High);

	FreeRam = 0;
	UsedRam = TotalRam;

	MMap_p mmap;
	for(uint32_t z = 0; z < (BOOTINF->MMapEnts); z++) {
		mmap = (MMap_p) ((uint32_t)BOOTINF->MMapAddr + (z * 24));
		if(mmap->Type == 1) {
			//Just incase the address and lengnth are not page aligned
			//Also increase Addr and decrease Len or we will exceed usable ram area
			uint64_t AlignedAddr = mmap->Base;
			uint64_t AlignedLen = mmap->Length;

			if(AlignedAddr % PAGESIZE) {
				AlignedAddr += (PAGESIZE - (AlignedAddr % PAGESIZE));
				AlignedLen -= (PAGESIZE - (AlignedAddr % PAGESIZE));
			}

			if(AlignedLen % PAGESIZE)
				AlignedLen -= (PAGESIZE - (AlignedLen % PAGESIZE));

			uint32_t PageAddr = (uint32_t) (AlignedAddr & 0xFFFFFFFF);
			uint32_t PageLen = (uint32_t) (AlignedLen & 0xFFFFFFFF);

			if(PageAddr < 0x10000) {
				uint32_t PageOff = 0x10000 - PageAddr;
				PageAddr += PageOff;
				if(PageLen > PageOff) {
					PageLen = PageLen - PageOff;
					_PMM_free(PageAddr, PageLen);
				}
			} else if(PageAddr > 0xFF000 && PageAddr < EndOfKernelData) {
				uint32_t PageOff = EndOfKernelData - PageAddr;
				PageAddr += PageOff;
				if(PageLen > PageOff) {
					PageLen = PageLen - PageOff;
					_PMM_free(PageAddr, PageLen);
				}
			} else
				_PMM_free(PageAddr, PageLen);
		}
	}
}

void _PMM_free(void* Base, uint32_t Length)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if((((uint32_t)Base) % PAGESIZE) || (Length % PAGESIZE)) {
		DEBUG_printf("ERROR FUNCTION \"%s\" Line %i", __func__, __LINE__);
		hlt();
	}
	for(uint32_t ent = 0; ent < PMM_Entries; ent++) {
		if(PMM[ent].Base == ((uint32_t)Base + Length)) {
			PMM[ent].Base -= Length;
			PMM[ent].Length += Length;
			FreeRam += Length;
			UsedRam -= Length;
			return;
		} else if((PMM[ent].Base + PMM[ent].Length) == ((uint32_t) Base)) {
			PMM[ent].Length += Length;
			FreeRam += Length;
			UsedRam -= Length;
			return;
		}
	}
	PMM[PMM_Entries].Base = (uint32_t) Base;
	PMM[PMM_Entries].Length = Length;
	PMM_Entries++;
	FreeRam += Length;
	UsedRam -= Length;
	// _PMM_defrag();
#ifdef DEBUG
	DEBUG_printf("\n\tTotal Ram %i KB\n\tFree  Ram %i KB\n\tUsed  Ram %i KB\n", (TotalRam / 1024), (FreeRam / 1024), (UsedRam / 1024));
#endif
}

void *_PMM_alloc(uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(Length % PAGESIZE) {
		DEBUG_printf("ERROR FUNCTION \"%s\" Line %i", __func__, __LINE__);
		hlt();
	}

	void* ret = 0;
	for(uint32_t e = 0; e < PMM_Entries; e++)
		if(PMM[e].Length >= Length) {
			ret = (void*) PMM[e].Base;
			PMM[e].Base += Length;
			PMM[e].Length -= Length;
			FreeRam -= Length;
			UsedRam += Length;
#ifdef DEBUG
	DEBUG_printf("\n\tTotal Ram %i KB\n\tFree  Ram %i KB\n\tUsed  Ram %i KB\n", (TotalRam / 1024), (FreeRam / 1024), (UsedRam / 1024));
#endif
			return ret;
		}
	return ret;
}

void _PMM_defrag()
{
#ifdef DEBUG_EXTREAM
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	//for (int x = 0; x < 2; x++) // Do this twice to make sure we get most of them.
		for (uint32_t e = 0; e < PMM_Entries; e++) {
			if(PMM[e].Length) {
				for (uint32_t t = 0; t < PMM_Entries; t++)
					if((PMM[t].Base + PMM[t].Length) == PMM[e].Base) {
						//Murge Entries
						PMM[e].Base = PMM[t].Base;
						PMM[e].Length += PMM[t].Length;
						PMM[t].Base = 0;
						PMM[t].Length = 0;
					}
			} else {
				// Physically Move Entries
				memcpy((void*) (&PMM[e]), (void*) (&PMM[e+1]), ((PMM_Entries - e) * 8));
				// Drop Entry Count
				PMM_Entries--;
				// This entry changed, dec e so we re-read it.
				e--;
			}
		}
}
