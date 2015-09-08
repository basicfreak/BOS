#ifndef				PMM_H_INCLUDED
	#define				PMM_H_INCLUDED
	
	#include <KLIB.h>
	#include <BOSBOOT.h>

	void _PMM_init(BootInfo_p);
	void _PMM_free(uint32_t, uint32_t);
	void *_PMM_malloc(uint32_t);

#endif