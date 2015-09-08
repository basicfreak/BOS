#ifndef				PHYSICAL_MEM_H_INCLUDED
	#define				PHYSICAL_MEM_H_INCLUDED

	#include <BOSBOOT.h>

	#define MAX_PMM_ENTS 0x00020000
	#define PAGESIZE 0x00001000

	typedef struct _PMM_ENTRY {
		uint32_t Base;
		uint32_t Length;
	}__attribute__((packed)) *PMMENT_p;

	extern void PMMBase(void);

	uint32_t TotalRam, FreeRam, UsedRam;

	void _PMM_init(BootInfo_p BOOTINF);
	void _PMM_free(void*, uint32_t);
	void *_PMM_alloc(uint32_t);

#endif