/*
./SRC/MEM/PHYS.c
*/

/*
Design and Theory

Stack is used loosly here it's probably more of a paging system than stack.

In a 32-BIT system there is a maximum of 4 GB (4,194,304 KB)
And we just happen to know the MMU on an x86 based system uses pages of 4 KB
So we have a maximum of 1,048,576 pages of memory

And just so happens that 2^20 = 1,048,576 so we require 40 bits per stack entry

And I want to control low memory (below 1 MB) seperatly because some things require low mem.
So the low memory is 1 MB (1024 KB) which is 256 pages which requires 16 bits per stack enrty.

We will use the same momory area to store both stacks. And we will keep track of free memory only.

Example of bitmap setup:

Lets use location 400 KB for low mem and 40 MB for high mem
and size of 120 KB for low mem and 2 GB for high

Low Mem Page			100			High Mem Page			10,240
Low Mem Location		01100100	High Mem location		00000010100000000000
Low Mem Page Count		30			High Mem Page Count		524,288
Low Mem Size			00011110	High Mem Size			10000000000000000000

Location
00000010100000000000 1st 16-bit data is high 16-bits next 8-bit data is 2 UNUSED the 2 BOOLs then low 4-bits
01100100 Last 8-bit data is 8-bits low memory				BIT 19 Set High Table Used and BIT 20 Set Low Table Used
--------------------------------------HIGH MEMORY AREA-----------------------------------------|---------LOW MEMORY AREA-------
01	02	03	04	05	06	07	08	09	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	
0 	0 	0 	0 	0 	0 	1 	0 	1 	0 	0 	0 	0 	0 	0 	0 	N 	N 	1 	1 	0 	0 	0 	0 	0 	1 	1 	0 	0 	1 	0 	0

Size
10000000000000000000 1st 16-bit data is high 16-bits next 8-bit data is 4 UNUSED then low 4-bits
00011110 Last 8-bit data is 8-bits low memory
--------------------------------------HIGH MEMORY AREA-----------------------------------------|---------LOW MEMORY AREA-------
01	02	03	04	05	06	07	08	09	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	
1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	N 	N 	N 	N 	0 	0 	0 	0 	0 	0 	0 	1 	1 	1 	1 	0

We can fit 64 entries per page so lets start this just after the kernel and page align one page for this.

As we keep track of only free memory we probably do not need even 64 entries but when we run out and need another we can allocate
another page as we just decrease the free size xor increase location to allocate a page.
*/

#include <PHYSMEM.H>

PHYSMEM_p	MemoryTable;

uint32_t	EoKernel	=		(uint32_t) end;

uint32_t	TotalMemory;
uint32_t	FreeMemory;
uint32_t	UsedMemory;
uint32_t	MemTableStart;		//Address of first Memory Table
uint8_t		MemTableCount;		//Count of pages used in table
uint8_t		MemTableCurrent;
uint16_t	MemEntCount_Low;	//Count of Low Memory Table Entries
uint16_t	MemEntCount_High;	//Count of High Memory Table Entries
// As we will have pagind on we can allocate all these in a row virtually

void		UpdateMemInfo			(void);
uint32_t	PHYSMEM_FreePages		(void);
uint32_t	getMemLocation_High		(uint16_t Entry);
uint8_t		getMemLocation_Low		(uint16_t Entry);
uint32_t	getMemSize_High			(uint16_t Entry);
uint8_t		getMemSize_Low			(uint16_t Entry);
void		setMemLocation_High		(uint16_t Entry, uint32_t Value);
void		setMemLocation_Low		(uint16_t Entry, uint8_t Value);
void		setMemSize_High			(uint16_t Entry, uint32_t Value);
void		setMemSize_Low			(uint16_t Entry, uint8_t Value);
void		removePages_High		(uint16_t Entry, uint32_t Count);
void		removePages_Low			(uint16_t Entry, uint32_t Count);


void		UpdateMemInfo			()
{
	FreeMemory = PHYSMEM_FreePages();
	UsedMemory = TotalMemory - FreeMemory;
#ifdef DEBUG_FULL
	DEBUG_printf("\n\tTotal RAM = %i KB\n\tUsed  RAM = %i KB\n\tFree  RAM = %i KB\n", TotalMemory, UsedMemory, FreeMemory);
#endif
}

bool		PHYSMEM_init			(BootInfo_p BOOTINF)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.2\t%s\tCompiled at %s on %s\tMAIN\t  Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	//multiboot_module_t *ModList = (multiboot_module_t *) (mbd->mods_addr + ((mbd->mods_count - 1) * 16));
	ModL_p ModList = BOOTINF->ModList;
	//MemTableStart = (ModList->mod_end + (0x1000 - (ModList->mod_end % 0x1000)));	//Lets Find the first page after the kernel and loaded modules.
	MemTableStart = (ModList[(BOOTINF->ModCount - 1)].ModEnd + 1);
	MemoryTable = (PHYSMEM_p) MemTableStart;		//The Memory Table is at the very next page after end of kernel and modules.
	MemTableCurrent = 0;
	MemTableCount = 1;
	memsetw((uint16_t *)MemTableStart, 0, 0x800);	//0x800 (2048) 16-bit words in 4096-bits
	MemEntCount_Low = 0;
	MemEntCount_High = 0;
	uint16_t entl = 0;
	uint16_t enth = 0;
	//TotalMemory = (mbd->mem_upper + mbd->mem_lower);
	TotalMemory = ((BOOTINF->Mem_Low + BOOTINF->Mem_High) / 1024);
	FreeMemory = 0;
	//Scan MultiBoot Memory Map For Free Memory
	//multiboot_memory_map_t *mmap;
	MMap_p mmap;
	//for(uint32_t z = 0; z < (mbd->mmap_length / 24); z++) {
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
			if(PageAddr <= (uint64_t) 0xFF) {
				//Low Memory Area
				if(PageAddr < 0x10) {
					uint32_t tempOff = (uint32_t)(0x10 - PageAddr);
					PageAddr = 0x10;
					PageLen = (PageLen - tempOff);
				}
				setMemLocation_Low(entl, (uint8_t) PageAddr);
				setMemSize_Low(entl, (uint8_t) PageLen);
				entl++;
			} else {
				//High Memory Area
				if(PageAddr < ((MemTableStart / 0x1000) + 1)) {
					// PageLen = PageLen - (((MemTableStart / 0x1000) + 1) - PageAddr);
					// PageAddr = ((MemTableStart / 0x1000) + 1);
					PageLen = (PageLen - ((MemTableStart / 0x1000) + 1));
					PageAddr = ((MemTableStart / 0x1000) + 1);
				}
				setMemLocation_High(enth, PageAddr);
				setMemSize_High(enth, PageLen);
				enth++;
			}
		}
	}

	UpdateMemInfo();
	return true;
}

uint32_t	PHYSMEM_FreePages		(void)
{
	uint32_t ret = 0;
	for(uint16_t entry = 0; entry < (MemTableCount * 64); entry++) {
		ret += getMemSize_High(entry);
		ret += (uint32_t) getMemSize_Low(entry);
	}
	return (ret * 4);
}


uint32_t	getMemLocation_High		(uint16_t Entry)
{
	uint32_t ret = 0;
	if(Entry < MemEntCount_High) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		if (MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low & 0x20)
			ret = (uint32_t) ((MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_High << 4) | (MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low & 0xF));
	}
	return ret;
}

uint8_t		getMemLocation_Low		(uint16_t Entry)
{
	uint8_t ret = 0;
	if(Entry < MemEntCount_Low) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		if (MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low & 0x10)
			ret = MemoryTable->ENTRY[Ent_TEnt].Location_LowMem;
	}
	return ret;
}

uint32_t	getMemSize_High			(uint16_t Entry)
{
	uint32_t ret = 0;
	if(Entry < MemEntCount_High) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		if (MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low & 0x20)
			ret = (uint32_t) ((MemoryTable->ENTRY[Ent_TEnt].Size_HighMem_High << 4) | (MemoryTable->ENTRY[Ent_TEnt].Size_HighMem_Low & 0xF));
	}
	return ret;
}

uint8_t		getMemSize_Low			(uint16_t Entry)
{
	uint8_t ret = 0;
	if(Entry < MemEntCount_Low) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		if (MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low & 0x10)
			ret = MemoryTable->ENTRY[Ent_TEnt].Size_LowMem;
	}
	return ret;
}

void		setMemLocation_High		(uint16_t Entry, uint32_t Value)
{
	if(Entry <= MemEntCount_High) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_High = (uint16_t) (Value >> 4);
		MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low = (uint8_t) ((MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low & 0x10) | ((Value & 0xF) | 0x20));
		if (Entry == MemEntCount_High)
			MemEntCount_High++;
	}
}

void		setMemLocation_Low		(uint16_t Entry, uint8_t Value)
{
	if(Entry <= MemEntCount_Low) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		MemoryTable->ENTRY[Ent_TEnt].Location_LowMem = (uint8_t) (Value & 0xFF);
		MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low |= 0x10; // Set Boolean for Low Mem Spot
		if (Entry == MemEntCount_Low)
			MemEntCount_Low++;
	}
}

void		setMemSize_High			(uint16_t Entry, uint32_t Value)
{
	if(Entry <= MemEntCount_High) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low |= 0x20;
		MemoryTable->ENTRY[Ent_TEnt].Size_HighMem_High = (uint16_t) (Value >> 4);
		MemoryTable->ENTRY[Ent_TEnt].Size_HighMem_Low = (uint8_t) (Value & 0xF);
		if (Entry == MemEntCount_High)
			MemEntCount_High++;
	}
}

void		setMemSize_Low			(uint16_t Entry, uint8_t Value)
{
	if(Entry <= MemEntCount_Low) {
		uint8_t Ent_Table = (uint8_t) (Entry / 64);
		uint8_t Ent_TEnt = (uint8_t) (Entry % 64);
		if (MemTableCurrent != Ent_Table) {
			MemoryTable = (PHYSMEM_p) (MemTableStart + (Ent_Table * (uint32_t) 0x1000));
			MemTableCurrent = Ent_Table;
		}
		MemoryTable->ENTRY[Ent_TEnt].Size_LowMem = (uint8_t) (Value & 0xFF);
		MemoryTable->ENTRY[Ent_TEnt].Location_HighMem_Low |= 0x10; // Set Boolean for Low Mem Spot
		if (Entry == MemEntCount_Low)
			MemEntCount_Low++;
	}
}

void		removePages_High		(uint16_t Entry, uint32_t Count)
{
	uint32_t tempSize = 0;
	uint32_t tempLoc = 0;
	if(Entry < MemEntCount_High && Count < (tempSize = getMemSize_High(Entry))) {
		tempLoc = getMemLocation_High(Entry);
		tempLoc = tempLoc + Count;
		tempSize = tempSize - Count;
		setMemLocation_High(Entry, tempLoc);
		setMemSize_High(Entry, tempSize);
	}
}

void		removePages_Low			(uint16_t Entry, uint32_t Count)
{
	if(Entry < MemEntCount_Low) {
		uint8_t tempSize = 0;
		uint8_t tempLoc = 0;
		tempLoc = getMemLocation_Low(Entry);
		tempSize = getMemSize_Low(Entry);
		tempLoc = (uint8_t) (tempLoc + Count);
		tempSize = (uint8_t) (tempSize - Count);
		setMemLocation_Low(Entry, tempLoc);
		setMemSize_Low(Entry, tempSize);
	}
}

void		*malloc_page			(uint32_t pages)
{
	void *ret = 0;
	for (uint16_t i = 0; i < MemEntCount_High; i++) {
		if(getMemSize_High(i) >= pages) {
			ret = (void *) (getMemLocation_High(i) * 0x1000);
			removePages_High(i, pages);
		}
	}
	UpdateMemInfo();
	return ret;
}

void		*calloc_page			(uint32_t pages)
{
	void *ret = 0;
	ret = malloc_page(pages);
	memsetw((uint16_t *) ret, (uint16_t) 0, (size_t) (0x800 * pages));
	return ret;
}

void		*malloc_page_low		(uint32_t pages)
{
	void *ret = 0;
	for (uint16_t i = 0; i < MemEntCount_Low; i++) {
		if(getMemSize_Low(i) >= pages) {
			ret = (void *) (uint32_t) (getMemLocation_Low(i) * 0x1000);
			removePages_Low(i, pages);
		}
	}
	UpdateMemInfo();
	return ret;
}

void		*calloc_page_low		(uint32_t pages)
{
	void *ret = 0;
	ret = malloc_page_low(pages);
	memsetw((uint16_t *) ret, (uint16_t) 0, (size_t) (0x800 * pages));
	return ret;
}

void		free_page				(void *page, uint32_t pages)
{
	DEBUG_printf("Page = 0x%x, Size = 0x%x\n", (uint32_t) page, pages);
	setMemLocation_High(MemEntCount_High, (uint32_t) ((int)page / 0x1000));
	setMemSize_High((uint16_t)(MemEntCount_High - 1), (uint32_t) pages);
	UpdateMemInfo();
}

uint32_t	get_TotalMem			()
{
	return (TotalMemory * 1024);
}

#ifdef DEBUG
	void PHYSMEM_Display_Stack()
	{
		DEBUG_print("\nPhysical Memory Stack Dump:\n");
		DEBUG_printf("\tLow Memory Stack Dump:\t\t(%i Entries)\n", MemEntCount_Low);
		DEBUG_print("\t\tStart Address\tSize Free\n\t\t  0xXXXXXX   \t  0xXXXXXX\n");
		for(uint16_t DisLow = 0; DisLow < MemEntCount_Low; DisLow++) {
			DEBUG_printf("\t\t  0x%x    \t  0x%x\n", getMemLocation_Low(DisLow),
					getMemSize_Low(DisLow));
		}
		DEBUG_printf("\tHigh Memory Stack Dump:\t\t(%i Entries)\n", MemEntCount_High);
		DEBUG_print("\t\tStart Address\tSize Free\n\t\t  0xXXXXXX   \t  0xXXXXXX\n");
		for(uint16_t DisHigh = 0; DisHigh < MemEntCount_High; DisHigh++) {
			DEBUG_printf("\t\t  0x%x    \t  0x%x\n", getMemLocation_High(DisHigh),
					getMemSize_High(DisHigh));
		}
		DEBUG_printf("Physical Memory Stack Dump Complete\n\n");
	}
#endif