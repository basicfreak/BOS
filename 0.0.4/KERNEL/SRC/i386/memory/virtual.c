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
		VMM[1024].Entry[PageTable] = (PagePhysAddress | I86_WRITABLE | I86_PRESENT);
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
    __asm__ __volatile__ ("invlpg (%0)" : : "a" (VAddr));

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
    __asm__ __volatile__ ("invlpg (%0)" : : "a" (VAddr));
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
	DEBUG_printf("%s Task Attempted to %s a %s page!\nVirtual Address:\t0x%x\n", ((r->err_code & 0x04) ? ("User") : ("Kernel")), ((r->err_code & 0x02) ? ("Write") : ("Read")), ((r->err_code & 0x01) ? ("Present") : ("Non-Present")), (uint32_t)getCR2());
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
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t VAddr = (uint32_t) Virt;
	uint32_t PageTable = (VAddr / 0x400000);
	return (void*)((VMM[0].Entry[PageTable] & 0xFFFFF000));
}

void _VMM_umapTable(void* Virt)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t VAddr = (uint32_t) Virt;
	uint32_t PageTable = (VAddr / 0x400000);
	VMM[0].Entry[PageTable] = 0;
}

void _VMM_setTable(void *Virt, void *Table, bool User, bool Write)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t VAddr = (uint32_t) Virt;
	uint32_t PAddr = (uint32_t) Table;
	uint32_t PageTable = (VAddr / 0x400000);
	if(User)
		PAddr |= I86_USER;
	if(Write)
		PAddr |= I86_WRITABLE;
	VMM[0].Entry[PageTable] = (PAddr | I86_PRESENT);
}

void *_VMM_copyPDir()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	void* PDirLoc = _VMM_newPDir(); // Make A Blank Page Directory (With Kernel Mapped)
	_VMM_map((void*)0, PDirLoc, FALSE, TRUE);

	for(uint32_t x = 0xFFC00000; x < 0xFFF80000; x += 0x1000) {
		//Copy User Tables...
		if(_VMM_getPhys((void*)x)) {
			void* NewTable = _PMM_alloc(PAGESIZE);
			_VMM_map((void*)0x1000, NewTable, FALSE, TRUE);
			memcpy((void*)0x1000, (void*)x, PAGESIZE);
			((uint32_t*)0)[((x - 0xFFC00000) / 0x1000)] = (((uint32_t)NewTable) & (VMM[0].Entry[((x - 0xFFC00000) / 0x1000)] & 0x07));
		}
	}
	_VMM_umap((void*)0x1000);

	_VMM_umap((void*)0);
	return PDirLoc;
}

void _VMM_setCOW()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	for(int x = 0; x < 0x380; x++)
		VMM[0].Entry[x] &= 0xFFFFF005;
	setPageDir(getPageDir()); // Invalidate whole table.
}

void _VMM_setCOWOther(void* PDIR)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	for(int x = 0; x < 0x380; x++)
		((uint32_t*)PDIR)[x] &= 0xFFFFF005;
}

void *_VMM_newPDir()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	void *PDirLoc = _PMM_alloc(PAGESIZE); // Allocate DIR
	// VMM_p PDir = (VMM_p) 0;
	
	_VMM_map((void*)0, PDirLoc, FALSE, TRUE);
	memcpy((void*)0, (void*)(&VMM[0]), (PAGESIZE)); // Copy DIR
	memsetd((void*)0, 0, 0x380); // Clear User Pages


	for(int x = 0; x < 0x400; x++)
		(((uint32_t*)0)[x]) &= 0xFFFFF003;

	void* TempTableA = _PMM_alloc(PAGESIZE);
	_VMM_map((void*)0x1000, TempTableA, FALSE, TRUE);
	memcpy((void*)0x1000, (void*)(&VMM[0x3FF]), (PAGESIZE)); // Copy Kernel Info
	((uint32_t*)0x1000)[0x3FF] = (((uint32_t)PDirLoc) | I86_WRITABLE | I86_PRESENT); // Set Page Dir as 0xFFBFF000
	((uint32_t*)0)[0x3FE] = (((uint32_t)TempTableA) | I86_WRITABLE | I86_PRESENT);
	((uint32_t*)0)[0x3FF] = 0;


	for(int x = 0; x < 0x400; x++)
		(((uint32_t*)0x1000)[x]) &= 0xFFFFF003;

	void* TempTableB = _PMM_alloc(PAGESIZE);
	_VMM_map((void*)0x1000, TempTableB, FALSE, TRUE);
	((uint32_t*)0)[0x3FF] = (((uint32_t)TempTableB) | I86_WRITABLE | I86_PRESENT);
	memcpy((void*)0x1000, (void*)(0), (PAGESIZE));
	((uint32_t*)0x1000)[0x3FE] = (((uint32_t)TempTableA) | I86_WRITABLE | I86_PRESENT); // Set Page Dir as 0xFFBFF000
	((uint32_t*)0x1000)[0x3FF] = (((uint32_t)TempTableB) | I86_WRITABLE | I86_PRESENT); // Set Page Dir as 0xFFBFF000

	return PDirLoc;
}

void _VMM_mapOther(void* PDIR, void* Virt, void* Phys, bool User, bool Write)
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

	_VMM_map((void*)0, PDIR, FALSE, TRUE);

	//Make Sure Table Is Allocatted In The Directory! 
	if(!(((uint32_t*)0)[PageTable] & I86_PRESENT)) {
		//Allocate Page For Address
		// uint32_t PageVirtAddress = (0xFFC00000 + (PAGESIZE * PageTable));
		uint32_t PagePhysAddress = (uint32_t) _PMM_alloc(PAGESIZE);
		((uint32_t*)0)[PageTable] = (PagePhysAddress | I86_WRITABLE | I86_PRESENT);
		_VMM_map((void*)0x1000, (void*)(((uint32_t*)0)[0x3FF] & 0xFFFFF000), FALSE, TRUE);
		((uint32_t*)0x1000)[PageTable] = (PagePhysAddress | I86_WRITABLE | I86_PRESENT);
		// VMM[1024].Entry[PageTable] = (PagePhysAddress | 0x03);
		// _VMM_map(PageVirtAddress, PagePhysAddress, FALSE, TRUE);
		// DEBUG_printf("A");
		_VMM_map((void*)0x1000, (void*)(PagePhysAddress), FALSE, TRUE);
		memset((void*)0x1000, 0, PAGESIZE);
		// DEBUG_printf("B");
	}
	if(User && !(((uint32_t*)0)[PageTable] & I86_USER))
		((uint32_t*)0)[PageTable] |= I86_USER;
	//Ok now it is safe to map the page.
	_VMM_map((void*)0x1000, (void*)(((uint32_t*)0)[PageTable] & 0xFFFFF000), FALSE, TRUE);
	((uint32_t*)0x1000)[PageTableEnt] = (PAddr | I86_PRESENT);
	_VMM_umap((void*)0x1000);
	_VMM_umap((void*)0);
}

void _VMM_umapOther(void* PDIR, void* Virt)
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

	_VMM_map((void*)0, PDIR, FALSE, TRUE);

	if((((uint32_t*)0)[PageTable] & 1)) {
		_VMM_map((void*)0x1000, (void*)(((uint32_t*)0)[PageTable] & 0xFFFFF000), FALSE, TRUE);
		((uint32_t*)0x1000)[PageTableEnt] = 0;
		_VMM_umap((void*)0x1000);
	}
	_VMM_umap((void*)0);
}

void *_VMM_getPhysOther(void* PDIR, void* Virt)
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
	void* ret = 0;
	_VMM_map((void*)0, PDIR, FALSE, TRUE);

	if((((uint32_t*)0)[PageTable] & 1)) {
		_VMM_map((void*)0x1000, (void*)(((uint32_t*)0)[PageTable] & 0xFFFFF000), FALSE, TRUE);
		ret = (void*) (((uint32_t*)0x1000)[PageTableEnt] & 0xFFFFF000);
		_VMM_umap((void*)0x1000);
	}
	_VMM_umap((void*)0);
	return ret;
}