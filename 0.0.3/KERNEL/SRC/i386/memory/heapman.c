
#include <i386/memory/heapman.h>
#include <i386/memory/virtman.h>

typedef struct Heap_Data_Struct {
	uint8_t BITMAP[455];
	uint8_t DATA[3640];
	uint8_t UNUSED;
} __attribute__((packed)) Heap_t, *Heap_p;


void _HMM_setBit(Heap_p MyHeap, uint32_t bit);
void _HMM_clearBit(Heap_p MyHeap, uint32_t bit);

uint32_t HeapBase = 0xA0000000;
uint32_t PageBase = 0xB0000000;
uint32_t KHeapBase = 0xD0000000;
uint32_t KPageBase = 0xE0000000;
uint32_t LengthBase = 0x10000000;

void *malloc(uint32_t Length)
{
	/*4096 / 4096 = 1
	4096 % 4096 = 0
	4095 / 4096 = 0
	4095 % 4096 = 4095
	4097 / 4096 = 1
	4097 % 4096 = 1*/
	uint32_t cs = 0;
	asm ("movl %%cs, %0" : "=r" (cs) );
	uint32_t PageAddr;
	uint32_t HeapAddr;
	if(cs == 0x08) {
		PageAddr = KPageBase;
		HeapAddr = KHeapBase;
	} else {
		PageAddr = PageBase;
		HeapAddr = HeapBase;
	}
	uint32_t PageMax = PageAddr + LengthBase;
	uint32_t HeapMax = HeapAddr + LengthBase;
	if(Length >= 3027) // if it is over 3KB give it a full page

		for(; PageAddr < PageMax; PageAddr += 0x1000)
			if(!getPhysical(PageAddr)) {
				uint32_t tLength = Length / 0x1000;
				if (Length % 0x1000)
					tLength++;
				if(cs == 0x08) _VMM_malloc(PageAddr, tLength, FALSE, TRUE);
				if(cs != 0x08) _VMM_malloc(PageAddr, tLength, TRUE, TRUE);
				return (void*)PageAddr;
			}
	for(; HeapAddr < HeapMax; HeapAddr += 0x1000) {
		if(!getPhysical(HeapAddr)) {
			// Need New Heap Page
			if(cs == 0x08) _VMM_malloc(HeapAddr, 1, FALSE, TRUE);
			if(cs != 0x08) _VMM_malloc(HeapAddr, 1, TRUE, TRUE);
			memset((void*)HeapAddr, 0, 455); // Clear the Bitmap
		}
		// See if there is enough room on this Heap
		uint32_t FreeCount = 0;
		uint32_t StartOfFree = 0;
		Heap_p MyHeap = (Heap_p) HeapAddr;
		for(uint32_t BitMap = 0; BitMap < 455; BitMap++) {
			if(MyHeap->BITMAP[BitMap] == 0) {
				FreeCount += 8;
				if(!StartOfFree)
					StartOfFree = (BitMap * 8);
			} else {
				FreeCount = 0;
				StartOfFree = 0;
			}
			if (FreeCount >= Length) {
				// We Found Space!
				for(uint32_t x = StartOfFree; x < Length + StartOfFree; x++)
					_HMM_setBit((Heap_p)HeapAddr, x);
				return (void*) (HeapAddr + 455 + StartOfFree);
			}
		}
	}
	return (void*)0;
}

void *calloc(uint32_t Length)
{
	void* addr = malloc(Length);
	if(addr)
		memset(addr, 0, (size_t)Length);
	return addr;
}

void free(void* Base, uint32_t Length)
{
	if(Length >= 3027) {
		uint32_t tLength = Length / 0x1000;
		if (Length % 0x1000)
			tLength++;
		_VMM_free((uint32_t)Base, tLength);
	} else {
		uint32_t HeapAddr = ((uint32_t) Base & 0xFFFFF000);
		uint32_t HeapOff = (((uint32_t)Base & 0xFFF) - 455);
		for(uint32_t x = HeapOff; x < Length + HeapOff; x++)
			_HMM_clearBit((Heap_p)HeapAddr, x);
	}
}

void _HMM_setBit(Heap_p MyHeap, uint32_t bit)
{
	//This way keeps GCC from bitching at me ;)
	MyHeap->BITMAP[bit / 8] = (uint8_t) ((MyHeap->BITMAP[bit / 8]) | (1 << (bit % 8)));
}

void _HMM_clearBit(Heap_p MyHeap, uint32_t bit)
{
	MyHeap->BITMAP[bit / 8] &= (uint8_t)~(1 << (bit % 8));
}