#include <i386/memory/virtual.h>
#include <i386/memory/physical.h>

VMM_p VMM = (VMM_p) ((uint32_t)&VMMBase);

enum I86_PAGE_FLAGS {
	I86_PRESENT			=	1,			//0000000000000000000000000000001
	I86_WRITABLE		=	2,			//0000000000000000000000000000010
	I86_USER			=	4,			//0000000000000000000000000000100
	I86_WRITETHOUGH		=	8,			//0000000000000000000000000001000
	I86_PWT				=	8,
	I86_NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
	I86_PCD				=	0x10,
	I86_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PAT				=	0x80,		//0000000000000000000000010000000
	I86_4MB				=	0x80,
	I86_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
};

void _VMM_init(BootInfo_p BOOTINF)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t tempVirt;
	for(tempVirt = 0x10000; tempVirt < 0x100000; tempVirt += PAGESIZE)
		_VMM_umap((void*)tempVirt);
// DEBUG_printf("END = 0x%x KVirtBase = 0x%x END-KVirtBase = 0x%x\n", (uint32_t)end, (uint32_t)KVirtBase, (((uint32_t)end) - ((uint32_t)KVirtBase)) );
	if((((uint32_t) end) - ((uint32_t)KVirtBase)) < 0x400000)
		for(tempVirt = 0x400000; tempVirt > (((uint32_t) &end) - ((uint32_t)&KVirtBase)); tempVirt -= PAGESIZE)
			_VMM_umap((void*)tempVirt);
	else if((((uint32_t) end) - ((uint32_t)KVirtBase)) > 0x400000)
		for(tempVirt = 0x400000; tempVirt < (((uint32_t) &end) - ((uint32_t)&KVirtBase)); tempVirt += PAGESIZE)
			_VMM_map((void*)(tempVirt + ((uint32_t)KVirtBase)), (void*)tempVirt, FALSE, TRUE);

	if(BOOTINF->ModCount) { // Identity Map Mods!
		ModL_p ModList = BOOTINF->ModList;
		tempVirt = ((ModList[(BOOTINF->ModCount - 1)].ModEnd + 1));
		for(tempVirt = ((((uint32_t)end) - ((uint32_t)KVirtBase)) & 0xFFFFF000); tempVirt <= ((ModList[(BOOTINF->ModCount - 1)].ModEnd + 0x1001)); tempVirt += PAGESIZE)
			_VMM_map((void*)tempVirt, (void*)tempVirt, FALSE, TRUE);
	}
}

void _VMM_map(void* Virt, void* Phys, bool User, bool Write)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t VAddr = (uint32_t) Virt;
	uint32_t PAddr = (uint32_t) Phys;
	if((VAddr % PAGESIZE) || (PAddr % PAGESIZE)) {
		DEBUG_printf("ERROR FUNCTION \"%s\" Line %i Virt = 0x%x, Phys = 0x%x\n", __func__, __LINE__, VAddr, PAddr);
		hlt();
	}
	if(User)
		PAddr |= I86_USER;
	if(Write)
		PAddr |= I86_WRITABLE;
#ifdef DEBUG_EXTREAM
	DEBUG_printf("0x%x\t0x%x\t0x%x\t0x%x\n", PAddr, VAddr, (VAddr / 0x400000), ((VAddr % 0x400000) / PAGESIZE));
#endif
	uint32_t PageTable = (VAddr / 0x400000);
	uint32_t PageTableEnt = ((VAddr % 0x400000) / PAGESIZE);

	//Make Sure Table Is Allocatted In The Directory! 
	if(!(VMM[0].Entry[PageTable] & I86_PRESENT)) {
		//Allocate Page For Address
		// uint32_t PageVirtAddress = (0xFFC00000 + (PAGESIZE * PageTable));
		uint32_t PagePhysAddress = (uint32_t) _PMM_alloc(PAGESIZE);
		VMM[0].Entry[PageTable] = (PagePhysAddress | I86_WRITABLE | I86_PRESENT);
		// VMM[1024].Entry[PageTable] = (PagePhysAddress | 0x03);
		// _VMM_map(PageVirtAddress, PagePhysAddress, FALSE, TRUE);
		// DEBUG_printf("A");
		memset((void*)(&VMM[PageTable + 1]), 0, PAGESIZE);

		// DEBUG_printf("B");
	}
	if(User && !(VMM[0].Entry[PageTable] & I86_USER))
		VMM[0].Entry[PageTable] |= I86_USER;
	//Ok now it is safe to map the page.
	VMM[PageTable + 1].Entry[PageTableEnt] = (PAddr | I86_PRESENT);

		// DEBUG_printf("C\n");
}

void _VMM_umap(void* Virt)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t VAddr = (uint32_t) Virt;
	if(VAddr % PAGESIZE) {
		DEBUG_printf("ERROR FUNCTION \"%s\" Line %i", __func__, __LINE__);
		hlt();
	}
	uint32_t PageTable = (VAddr / 0x400000);
	uint32_t PageTableEnt = ((VAddr % 0x400000) / PAGESIZE);
	if((VMM[0].Entry[PageTable] & 1))
		VMM[PageTable + 1].Entry[PageTableEnt] = 0;
}

void *_VMM_getPhys(void* Virt)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t VAddr = (uint32_t) Virt;
	if((VAddr % PAGESIZE)) {
		DEBUG_printf("ERROR FUNCTION \"%s\" Line %i", __func__, __LINE__);
		hlt();
	}
	uint32_t PageTable = (VAddr / 0x400000);
	uint32_t PageTableEnt = ((VAddr % 0x400000) / PAGESIZE);

	if(!(VMM[0].Entry[PageTable] & I86_PRESENT))
		return 0;

	return (void*) (VMM[PageTable + 1].Entry[PageTableEnt] & VMMFRAME);
}

void _VMM_PageFaultManager(regs *r)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
	DEBUG_printf("User Task Attempted to %s a %s page!\nVirtual Address:\t0x%x\n", ((r->err_code & 0x04) ? ("User") : ("Kernel")), ((r->err_code & 0x02) ? ("Write") : ("Read")), ((r->err_code & 0x01) ? ("Present") : ("Non-Present")), (uint32_t)getCR2());
#endif
	//int 0x0E = page fault
	if(!(r->err_code & 0x04)) {
		//KERNEL TASK PAGEFAULTED!!!!
#ifdef DEBUG_FULL
	DEBUG_print("System Haulted.\n");
#endif
		hlt();
	} else if((r->err_code & 0x07) == 0x07) {
#ifdef DEBUG_FULL
	DEBUG_print("Copy On Write\n");
#endif
		// Page is present. And We attempted to write - in user mode
		void *NewPage = _PMM_alloc(PAGESIZE);
		_VMM_map(0, NewPage, FALSE, TRUE);
		memcpyd(0, (void*)(((uint32_t)(getCR2())) & 0xFFFFF000) , (PAGESIZE >> 2));
		_VMM_umap(0);
		_VMM_map((void*)(((uint32_t)(getCR2())) & 0xFFFFF000), NewPage, TRUE, TRUE);
	} /*else if(((((uint32_t)(getCR2()) >> 12 ) << 12) == (uint32_t)((r->useresp >> 12) << 12)) && (!(r->err_code & 0x01))) {
		//Stack Over/Under Flow
		//For now don't worry about it just map the page
		//BIG SECURITY HOLE AS USER CAN CHANGE ESP TO ANYTHING
		//WHICH COULD SCREW THINGS UP BIG TIME!!
		void *NewPage = _PMM_alloc(PAGESIZE);
		_VMM_map((void*)(((uint32_t)(getCR2()) >> 12 ) << 12), NewPage, TRUE, TRUE);
	}*/ else {
		//kill thread
#ifdef DEBUG_FULL
	DEBUG_print("Task Killed.\n");
#endif
		killCurrentThread();
	}
}

void *_VMM_getTable(void *Virt)
{
	uint32_t VAddr = (uint32_t) Virt;
	uint32_t PageTable = (VAddr / 0x400000);
	return (void*)((VMM[0].Entry[PageTable] & 0xFFFFF000));
}

void _VMM_umapTable(void* Virt)
{
	uint32_t VAddr = (uint32_t) Virt;
	uint32_t PageTable = (VAddr / 0x400000);
	VMM[0].Entry[PageTable] = 0;
}

void _VMM_setTable(void *Virt, void *Table, bool User, bool Write)
{
	uint32_t VAddr = (uint32_t) Virt;
	uint32_t PAddr = (uint32_t) Table;
	uint32_t PageTable = (VAddr / 0x400000);
	if(User)
		PAddr |= I86_USER;
	if(Write)
		PAddr |= I86_WRITABLE;
	VMM[0].Entry[PageTable] = (PAddr | I86_PRESENT);
}
