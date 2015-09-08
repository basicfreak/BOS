/*
./DRIVERSRC/SYSTEM/MEM/PHYSICAL.C
*/

#include "PHYSICAL.H"
#include <STDIO.H>

//#define DEBUG

uint32_t probeRAM(void);
bool mmap_test (int bit);
void set_used(uint32_t page);
int mmap_first_free(void);
int mmap_first_free_s (size_t size);


uint32_t *Mem_Map;
uint32_t Total_Ram;

uint32_t probeRAM() //in KB
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 20) probeRAM() = ");
#endif
	uint32_t out = 0xFFF; 									//we will start at 4MB and assume you have 4MB of RAM Minimal 1024 bytes = 1kb 1024 kb = 1MB
	bool issue = false;
	while (!issue) {
		if (out < 0x4000)
			out += 0x1;									// add 1kb to memory location if at less than 16MB
		else if (out < 0x20000)
			out += 0x400;								// else add 1mb to memory location if less than 128MB
		else if (out < 0x100000)
			out += 0x4000;								// else add 16MB to location if less than 1024MB\1GB
		else
			out += 0x20000;								// else add 128MB to location
			
		char temp = (char) RAM[out*0x400];							//Save the byte that was here to temp
		/*	Do a bit-shift to find usable RAM	*
		*		  01010101 -> 10101010			*/
		RAM[out*0x400] = 0x55; //01010101
		if (RAM[out*0x400] != 0x55) issue = true;
		RAM[out*0x400] = 0xAA; //10101010
		if (RAM[out*0x400] != 0xAA) issue = true;
		
		RAM[out*0x400] = (uint8_t) temp;								//Restore the byte that was here from temp
	}
#ifdef DEBUG
	txf(1, "0x%x\n\r", out);
#endif
	return out;
}

bool mmap_test (int bit)		// Some Reason hardware divide does not work here...
{
	int divsoft = bit;
	int divsoftnum = 0;
	while (divsoft > 31) {
		divsoftnum++;
		divsoft = divsoft - 32;
	}
/*#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 53) mmap_test(0x%x)\n\r", (uint32_t) bit);
	txf(1, "\tMem_Map[(0x%x / 32)] & (uint32_t) (0x01 << (0x%x %% 32))\n\r", (uint32_t) bit, (uint32_t) bit);
	txf(1, "\tMem_Map[(0x%x / 32)] & (uint32_t) (0x01 << (0x%x))\n\r", (uint32_t) bit, (uint32_t) (bit%32));
	txf(1, "\tMem_Map[(0x%x / 32)] & 0x%x)\n\r", (uint32_t) bit, (uint32_t) (1 << (bit%32)));
	txf(1, "\tMem_Map[0x%x] & 0x%x)\n\r", (uint32_t) divsoftnum, (uint32_t) (1 << (bit%32)));
	txf(1, "\t0x%x & 0x%x)\n\r", Mem_Map[divsoftnum], (uint32_t) (1 << (bit%32)));
	txf(1, "\t0x%x\n\r", (Mem_Map[divsoftnum] & (uint32_t) (1 << (bit % 32))));
#endif*/
	return (bool) (Mem_Map[divsoftnum] & (uint32_t) (1 << (bit % 32)));//((Mem_Map[(bit / 32)]) & (uint32_t) (1 << (bit % 32)));
}

void set_used(uint32_t page)
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 73) set_used(0x%x)\n\r", page);
#endif
	Mem_Map[page / 32] |= (uint32_t) (1 << (uint32_t) (page % 32));
	_mmngr_free_blocks--;
	_mmngr_used_blocks++;
}

int mmap_first_free ()
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 83) mmap_first_free () = ");
#endif
	for (uint32_t i=0; i<(_mmngr_max_blocks/32); i++)
		if (Mem_Map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {				// test each bit in the dword
				int bit = 1 << j;
				if (! (uint32_t) (Mem_Map[i] & (uint32_t) bit) ) {
#ifdef DEBUG
	txf(1, "0x%x\n\r", (uint32_t) ((uint32_t) (i*32)+(uint32_t) j));
#endif
					return (int) ((uint32_t) (i*32)+(uint32_t) j);
				}
			}
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
	return -1;
}

int mmap_first_free_s (size_t size)
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 105) mmap_first_free_s (0x%x) = ", (uint32_t) size);
#endif
	if (size==0) {
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
		return -1;
	}
	if (size==1)
		return mmap_first_free ();
	for (uint32_t i=0; i<_mmngr_max_blocks/32; i++) {
			if (Mem_Map[i] != 0xffffffff)
				for (uint32_t j=0; j<32; j++) {	// test each bit in the dword
					uint32_t bit = (uint32_t) (1<<j);
					if (! (Mem_Map[i] & bit) ) {
						uint32_t startingBit = ((i*32)+bit);		//get the free bit in the dword at index i
						uint32_t free=0; //loop through each bit to see if its enough space
						for (uint32_t count=0; (count<=((uint32_t)size)) && ((startingBit+count) < _mmngr_max_blocks);count++) {	// Make sure we do not overflow?
							bool testbit = mmap_test((int)(startingBit+count));
							if (!testbit) {
								free++;	// this bit is clear (free frame)
							} else {
								count = (uint32_t) (size+1);
								free = 0;
							}
							if (free==(uint32_t) size) {
#ifdef DEBUG
	txf(1, "0x%x\n\r", ((i*32)+j));
#endif
								return (int) (i*32+j); //free count==size needed; return index
							}
						}
					}
				}
		}
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
	return -1;
}

void *malloc(uint32_t pages)	// Just Allocate
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 149) malloc(0x%x) = ", pages);
#endif
	if (_mmngr_free_blocks <= pages) {
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
		return 0;
	}	//not enough space

	int frame = mmap_first_free_s ((size_t)pages);

	if (frame == -1) {
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
		return 0;
	}	//not enough space

	for (uint32_t i=0; i<pages; i++)
		set_used ((uint32_t) frame+(uint32_t)i);

	uint32_t addr = (uint32_t)((uint32_t)frame * (uint32_t)PMMNGR_BLOCK_SIZE);
#ifdef DEBUG
	txf(1, "0x%x\n\r", addr);
#endif
	return (void*)addr;
}

void *calloc(uint32_t pages)	// Clear and Allocate
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 180) calloc(0x%x)\n\r", pages);
#endif
	uint32_t* location = (uint32_t*) malloc(pages);
	memset (location, 0, (size_t)((size_t) pages * 0x1000));
	return (void *) location;
}

void free(void *in)		// Deallocate Page
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 190) free(0x%x)\n\r", page);
#endif
	uint32_t page = 0;
	page = ((uint32_t) in / 0x1000);
	Mem_Map[page / 32] &= (uint32_t) (~ (1 << (page % 32)));
	_mmngr_free_blocks++;
	_mmngr_used_blocks--;
}

void _PMem_init()
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 200) _PMem_init()\n\r");
#endif
	RAM = 0;
	Mem_Map = (uint32_t *)MMAP_LOCATION;
	memset(&Mem_Map[0], (unsigned short) 0xFF, (size_t) MMAP_SIZE);	// Allocate all RAM as Used
	Total_Ram = probeRAM();
	memset(&Mem_Map[0x20], 0, (size_t)(Total_Ram - 0x1000));  // Free available RAM after 4MB..?
	_mmngr_memory_size	=	Total_Ram;
	_mmngr_memory_map	=	(uint32_t*) MMAP_LOCATION;
	_mmngr_max_blocks	=	(Total_Ram*0x400) / PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks	=	0x400;
	_mmngr_free_blocks	=	(Total_Ram / 4);
	printf("\tTotal RAM:\t%iKB...", _mmngr_memory_size);
#ifdef DEBUG
	txf(1, "\tTotal RAM:\t%iKB\n\r", _mmngr_memory_size);
	txf(1, "\tFree RAM:\t%iKB\n\r", _mmngr_memory_size-(_mmngr_used_blocks*4));
	txf(1, "\tUsed RAM:\t%iKB\n\r", _mmngr_used_blocks*4);
#endif
}