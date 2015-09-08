/*
./DRIVERSRC/SYSTEM/MEM/PHYSICAL.H
*/


#ifndef				PHYSICAL_H_INCLUDED
	#define				PHYSICAL_H_INCLUDED
	
	#include <FORMATTING.H>
	
	#define PMMNGR_BLOCKS_PER_BYTE	8
	#define PMMNGR_BLOCK_SIZE		4096
	#define PMMNGR_BLOCK_ALIGN		PMMNGR_BLOCK_SIZE
	#define MMAP_LOCATION			0x180000			//1.5MB mark give my Kernel 512KB
	//4294967296 / 4KB Pages = 1048576 Pages / 8 Pages per Byte = 131072 Bytes / 4 Bytes Per uint32_t = 32768 uint32_t for 4GB ram map
	#define MMAP_SIZE				0x020000			//128KB MAP ends at 1.625MB Mark
	
	uint8_t *RAM;
	
	//Using these to make this code reverse compatible... With previous builds and files in existence
	uint32_t _mmngr_memory_size;
	uint32_t _mmngr_used_blocks;
	uint32_t _mmngr_max_blocks;
	uint32_t* _mmngr_memory_map;
	uint32_t _mmngr_free_blocks;
	
	void *malloc(uint32_t pages);
	void *calloc(uint32_t pages);
	void free(void *in);
	void _PMem_init(void);
	
#endif