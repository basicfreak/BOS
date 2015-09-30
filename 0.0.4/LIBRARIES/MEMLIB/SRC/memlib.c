#include <stdlib.h>
#include <memlib.h>
#include <systemcalls.h>

#define _HEAPSTACK 0xCF780000
#define _HEAPSIZE 0x00080000
#define _HEAPFREEENTS 0x00000080
#define _HEAPALLOCENTS 0x0000FF80
#define _PAGESIZE 0x00001000

typedef struct HeapStack {
	void* Start;
	uint32_t Length;
} __attribute__((packed)) *HStack_p;

void initHeap()
{
	for(uint32_t VAddr = _HEAPSTACK; VAddr < (_HEAPSTACK + _HEAPSIZE); VAddr += _PAGESIZE)
		_VMM_alloc(VAddr, TRUE);
	memset((void*)_HEAPSTACK, 0, _HEAPSIZE);
	((HStack_p)_HEAPSTACK)[0].Start = (void*)0x81000000;
	((HStack_p)_HEAPSTACK)[0].Length = (_HEAPSTACK - 0x81000000);
}

void* malloc(uint32_t Length)
{
	uint32_t Len = (Length / _PAGESIZE);
	if(Length % _PAGESIZE)
		Len++;

	for(int y = 0; y < _HEAPFREEENTS; y++) {
		if(((HStack_p)_HEAPSTACK)[y].Length >= (Len * _PAGESIZE)) {
			void* ret = ((HStack_p)_HEAPSTACK)[y].Start;
			for(int x = _HEAPFREEENTS; x < _HEAPALLOCENTS; x++) {
				if(!((HStack_p)_HEAPSTACK)[x].Length) {
					((HStack_p)_HEAPSTACK)[y].Start = ret;
					((HStack_p)_HEAPSTACK)[x].Length = (Len * _PAGESIZE);
				}
			}
			((HStack_p)_HEAPSTACK)[y].Start = (void*) ((uint32_t)(((HStack_p)_HEAPSTACK)[y].Start) + (Len * _PAGESIZE));
			((HStack_p)_HEAPSTACK)[y].Length -= (Len * _PAGESIZE);
			while(Len)
				_VMM_alloc((ret + (--Len * _PAGESIZE)), TRUE);
			return ret;
		}
	}
	return 0;
}

void* calloc(uint32_t Length)
{
	void* ret = malloc(Length);
	if(ret)
		memset(ret, 0, Length);
	return ret;
}

void free(void* Start)
{
	for(int x = _HEAPFREEENTS; x < _HEAPALLOCENTS; x++)
		if( (((HStack_p)_HEAPSTACK)[x].Start == Start) ) {
			uint32_t Len = (((HStack_p)_HEAPSTACK)[x].Length / _PAGESIZE);
			while (Len)
				_VMM_umapFree(((uint32_t)(((HStack_p)_HEAPSTACK)[x].Start) + (--Len * _PAGESIZE)));
			for(int y = 0; y < _HEAPFREEENTS; y++) {
				if( (uint32_t)(((HStack_p)_HEAPSTACK)[y].Start) == ((uint32_t)(((HStack_p)_HEAPSTACK)[x].Start) + ((HStack_p)_HEAPSTACK)[x].Length)) {
					((HStack_p)_HEAPSTACK)[y].Start = (void*) ((uint32_t)(((HStack_p)_HEAPSTACK)[y].Start) - ((HStack_p)_HEAPSTACK)[x].Length);
					((HStack_p)_HEAPSTACK)[y].Length += ((HStack_p)_HEAPSTACK)[x].Length;
					((HStack_p)_HEAPSTACK)[x].Start = 0;
					((HStack_p)_HEAPSTACK)[x].Length = 0;
					return;
				} else if ( ((uint32_t)(((HStack_p)_HEAPSTACK)[y].Start) + ((HStack_p)_HEAPSTACK)[y].Length) ==  (uint32_t)(((HStack_p)_HEAPSTACK)[x].Start)) {
					((HStack_p)_HEAPSTACK)[y].Length += ((HStack_p)_HEAPSTACK)[x].Length;
					((HStack_p)_HEAPSTACK)[x].Start = 0;
					((HStack_p)_HEAPSTACK)[x].Length = 0;
					return;
				} else if (!((HStack_p)_HEAPSTACK)[y].Length) {
					((HStack_p)_HEAPSTACK)[y].Start = ((HStack_p)_HEAPSTACK)[x].Start;
					((HStack_p)_HEAPSTACK)[y].Length = ((HStack_p)_HEAPSTACK)[x].Length;
					((HStack_p)_HEAPSTACK)[x].Start = 0;
					((HStack_p)_HEAPSTACK)[x].Length = 0;
					return;
				}
			}
		}
}
