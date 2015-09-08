// ./SRC/i386/memory/virtual.c

#include <i386/memory/virtman.h>
#include <i386/memory/physman.h>

Paging_p CurrentDir = (Paging_p) 0xF0000000;
extern void _VMM_flush(void);

void _VMM_init(BootInfo_p BOOTINF)
{
	//This will clear the unused pages.
	uint32_t tempVirt;
	for(tempVirt = 0x10000; tempVirt < 0x100000; tempVirt += 0x1000)
		_VMM_unmap(tempVirt);
	if(BOOTINF->ModCount) {
		ModL_p ModList = BOOTINF->ModList;
		tempVirt = ((ModList[(BOOTINF->ModCount - 1)].ModEnd + 1) + 0xC0000000);
	}
	else tempVirt = ((uint32_t)end);
	tempVirt += 0x1000;
	if(tempVirt % 0x1000) tempVirt += (0x1000 - (tempVirt % 0x1000));
	for(;tempVirt <= 0x400000; tempVirt += 0x1000)
		_VMM_unmap(tempVirt);
	for(uint32_t x = 0xA0000; x < 0xF0000; x+=0x1000)
		_VMM_map(x, x, TRUE, TRUE);
}

void _VMM_map(uint32_t Physical, uint32_t Virtual, bool User, bool Write)
{
	uint32_t PhysEntry = Physical & 0xFFFFF000;

	uint32_t PageTable = (Virtual / 0x400000);
	uint32_t PageVirtAddress = (0xF0001000 + (0x1000 * PageTable));
	uint32_t PageTableEnt = ((Virtual % 0x400000) / 0x1000);

	//Make Sure Table Is Allocatted In The Directory!
	if(!(CurrentDir->entry[PageTable] & 0x03)) {
		//Allocate Page For Address
		uint32_t PagePhysAddress = (uint32_t) _PMM_malloc(1);
		CurrentDir->entry[PageTable] = (PagePhysAddress | 0x03);
		_VMM_map(PagePhysAddress, PageVirtAddress, FALSE, TRUE);
		memset((void*)PageVirtAddress, 0, 0x1000);
	}

	//Ok now it is safe to map the page.
	Paging_p CurrentTable = (Paging_p) PageVirtAddress;
	CurrentTable->entry[PageTableEnt] = (PhysEntry | I86_PRESENT);
	if(User)
		CurrentTable->entry[PageTableEnt] |= I86_USER;
	if(Write)
		CurrentTable->entry[PageTableEnt] |= I86_WRITABLE;

	_VMM_flush();

}

void _VMM_unmap(uint32_t Virtual)
{
	uint32_t PageTable = (Virtual / 0x400000);
	uint32_t PageVirtAddress = (0xF0001000 + (0x1000 * PageTable));
	uint32_t PageTableEnt = ((Virtual % 0x400000) / 0x1000);
	if((CurrentDir->entry[PageTable] & 0x03)) {
		Paging_p CurrentTable = (Paging_p) PageVirtAddress;
		CurrentTable->entry[PageTableEnt] = 0;
		_VMM_flush();
	}

}

void *_VMM_malloc(uint32_t Virtual, uint32_t Pages, bool user, bool write)
{
	uint32_t PhysAddr = (uint32_t) _PMM_malloc(Pages);

	if(PhysAddr)
		for(uint32_t x = 0; x < Pages; x++)
			_VMM_map((PhysAddr + (0x1000 * x)), (Virtual + (0x1000 * x)), user, write);

	
	return (void*)Virtual;
}

void _VMM_free(uint32_t Virtual, uint32_t Pages)
{
	uint32_t PhysBase = (uint32_t)getPhysical(Virtual);
	if(PhysBase) {
		uint32_t PhysPages = Pages;
		for(Pages++; Pages > 0; Pages--, Virtual+=0x1000)
			_VMM_unmap(Virtual);
		_PMM_free(PhysBase, PhysPages);
	}
}

void *getPhysical(uint32_t Virtual)
{
	uint32_t PageTable = (Virtual / 0x400000);
	uint32_t PageVirtAddress = (0xF0001000 + (0x1000 * PageTable));
	uint32_t PageTableEnt = ((Virtual % 0x400000) / 0x1000);
	if((CurrentDir->entry[PageTable] & 0x03)) {
		Paging_p CurrentTable = (Paging_p) PageVirtAddress;
		return (void*) (CurrentTable->entry[PageTableEnt] & 0xFFFFF000);
	}
	return (void*) 0;
}

void *mkPageDir()
{
	void *ret = 0;
	uint32_t KernelPageTable = (0xC0000000 / 0x400000);
	uint32_t KernelVirtPageTableAddr = (0xF0001000 + (0x1000 * KernelPageTable));
	Paging_p NewDIR = (Paging_p)_VMM_malloc(0xC0000000, 1, FALSE, TRUE);
	memset(NewDIR, 0, 0x1000);
	NewDIR->entry[0x300] = ((uint32_t) getPhysical(KernelVirtPageTableAddr) | 3);
	NewDIR->entry[0] = ((uint32_t) getPhysical(0xF0001000) | 3);
	Paging_p NewDirEntList = (Paging_p) _VMM_malloc(0xC0001000, 1, FALSE, TRUE);
	memset(NewDirEntList, 0, 0x1000);
	NewDIR->entry[0x3C0] = ((uint32_t) getPhysical(0xC0001000) | 3);
	NewDirEntList->entry[0] = ((uint32_t) getPhysical(0xC0000000) | 3);
	NewDirEntList->entry[1] = ((uint32_t) getPhysical(0xF0001000) | 3);
	NewDirEntList->entry[0x301] = ((uint32_t) getPhysical(KernelVirtPageTableAddr) | 3);
	NewDirEntList->entry[0x3C1] = ((uint32_t) getPhysical(0xC0001000) | 3);
	ret = getPhysical(0xC0000000);
	_VMM_unmap(0xC0000000);
	_VMM_unmap(0xC0001000);
	return ret;

	/*	uint32_t PhysEntry = Physical & 0xFFFFF000;
	PhysEntry |= 3;

	uint32_t PageTable = (Virtual / 0x400000);
	uint32_t PageVirtAddress = (0xF0001000 + (0x1000 * PageTable));
	uint32_t PageTableEnt = ((Virtual % 0x400000) / 0x1000);*/
}
