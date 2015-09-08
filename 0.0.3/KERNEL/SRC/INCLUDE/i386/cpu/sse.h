#ifndef				_SSE_H_INCLUDED
	#define				_SSE_H_INCLUDED
	
	#include <KLIB.h>
	
	typedef struct SSESAVEDATA {
		uint32_t Data[0x80]; //512 Bytes
	}__attribute__((packed)) ssed;

	void _SSE_init(void);
	void SSE_Save(void* ssedLocation);
	void SSE_Load(void* ssedLocation);
	
#endif