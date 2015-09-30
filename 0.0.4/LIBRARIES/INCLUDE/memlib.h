#ifndef _MEMLIB_H_INCLUDE
	#define _MEMLIB_H_INCLUDE

	#include <typedefines.h>

	void initHeap(void);
	void* malloc(uint32_t);
	void* calloc(uint32_t);
	void free(void*);

#endif