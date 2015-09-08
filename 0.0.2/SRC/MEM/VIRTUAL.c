/*
./SRC/MEM/VIRTUAL.c
*/

#include <VIRTUALMEM.H>
#include <PHYSMEM.H>
//#include <PAGEFAULT.H>

extern void enable_paging(void);
extern void disable_paging(void);

PageEntry	MakePage				(paddr physical, bool user, bool write);
void		Add_Table_To_Dir		(uint32_t index, ptbl_p table, pdir_p dir, bool user, bool write);
ptbl_p		MakeTable				(void);
pdir_p		MakeDirectory			(void);
void		setPaging				(bool enable);
void		setPageDirectroy		(pdir_p dir);
bool		Add_Page_To_Table		(uint32_t index, ptbl_p table, PageEntry page);

PageEntry	MakePage				(paddr physical, bool user, bool write)
{
	PageEntry e = I86_PRESENT;
	if (user)
		e |= I86_USER;
	if (write)
		e |= I86_WRITABLE;
	e = (PageEntry) ((PageEntry) (e & (PageEntry)(~I86_FRAME)) | (PageEntry) physical);
	return e;
}

void		Add_Table_To_Dir		(uint32_t index, ptbl_p table, pdir_p dir, bool user, bool write)
{
	uint32_t *entry = (uint32_t*) &dir->table [index];
	*entry = I86_PRESENT;
	if (user)
		*entry |= I86_USER;
	if (write)
		*entry |= I86_WRITABLE;
	*entry = (uint32_t) ((*entry & (uint32_t) (~I86_FRAME)) | (uint32_t)table);
}

ptbl_p		MakeTable				()
{
	ptbl_p ret = (ptbl_p) calloc_page(1);
#ifdef DEBUG_FULL
	DEBUG_printf("(%s:%i) %s()\t=\t0x%x\n", __FILE__, __LINE__, __func__, ret);
#endif
	return ret;
}

pdir_p		MakeDirectory			()
{
	pdir_p ret = (pdir_p) calloc_page(1);
#ifdef DEBUG_FULL
	DEBUG_printf("(%s:%i) %s()\t=\t0x%x\n", __FILE__, __LINE__, __func__, ret);
#endif
	return ret;
}

void		setPaging				(bool enable)
{
	if (enable)
		enable_paging();
	else
		disable_paging();
}

void		setPageDirectroy		(pdir_p dir)
{
	__asm__ __volatile__ ("mov %0, %%cr3":: "b"(dir));
}

bool		Add_Page_To_Table		(uint32_t index, ptbl_p table, PageEntry page)
{
	table = (ptbl_p) ((uint32_t) table & (uint32_t) I86_FRAME);
	if(!(table->page[index] = page))
		return FALSE;
	return TRUE;
}

bool		VIRTUALMEM_init			()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.2\t%s\tCompiled at %s on %s\tMAIN\t  Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	// Create the Kernel's Page Directory.
	if(!(KERNEL_PAGE_DIR = MakeDirectory()))
		return FALSE;
	uint32_t x;
	for(x = 0; x < (get_TotalMem() / (PAGE_SIZE)); x++)
		if(!AddPage(KERNEL_PAGE_DIR, (paddr) (x * PAGE_SIZE), (vaddr) (x * PAGE_SIZE), FALSE, TRUE))
			return FALSE;
	//C0000000
	for(x = 0xC0000; x < 0xC07E9; x++)
		if(!AddPage(KERNEL_PAGE_DIR, (paddr) (x * PAGE_SIZE), (vaddr) (x * PAGE_SIZE), FALSE, TRUE))
			return FALSE;
	//F8000000
	for(x = 0xF8000; x < 0xF87E9; x++)
		if(!AddPage(KERNEL_PAGE_DIR, (paddr) (x * PAGE_SIZE), (vaddr) (x * PAGE_SIZE), FALSE, TRUE))
			return FALSE;
	// Setup Page Fault Handler
	//_pageFault_init();
	// switch to our page directory
	setPageDirectroy (KERNEL_PAGE_DIR);
	// enable paging
	setPaging (true);
	return TRUE;
}

bool		AddPage					(pdir_p Dir, paddr Phys, vaddr Virt, bool user, bool write)
{
	// Calculate Variables...
	uint32_t PageTable = Virt  / (PAGES_PER_TABLE * PAGE_SIZE);
	uint32_t PageLoc = (Virt % (PAGES_PER_TABLE * PAGE_SIZE)) / PAGE_SIZE;
	// We Want a new page made (Phys = 0)
	if(!Phys) 
		Phys = (paddr) calloc_page(1);
	if(!Phys)
		return FALSE;
	PageEntry Page = MakePage(Phys, user, write);
	if(!Page)
		return FALSE;
	// If Page Table Does Not Exist Make It...
	if(!Dir->table[PageTable]) 
		Add_Table_To_Dir(PageTable, MakeTable(), Dir, user, write);
	if(!Add_Page_To_Table(PageLoc, Dir->table[PageTable], Page))
		return FALSE;
	return TRUE;
}

bool		DelPage					(pdir_p Dir, vaddr Virt, bool clean)
{
#ifdef DEBUG_FULL
	DEBUG_printf("(%s:%i) %s(0x%x, 0x%x, %s)\n", __FILE__, __LINE__, __func__, Dir, Virt, printBOOL(clean));
#endif
	if(!Dir)
		return FALSE;
	// Calculate Variables...
	uint32_t PageTable = (uint32_t) (Virt  / (PAGES_PER_TABLE * PAGE_SIZE));
	uint32_t PageLoc = (uint32_t) ((Virt % (PAGES_PER_TABLE * PAGE_SIZE)) / PAGE_SIZE);
	if(!Dir->table[PageTable])
		return FALSE;
	ptbl_p table = (ptbl_p) ((uint32_t) Dir->table[PageTable] & (uint32_t) I86_FRAME);
	if(clean)
		free_page((void *) (table->page[PageLoc] & I86_FRAME), 1);
	if((table->page[PageLoc] = 0))
		return FALSE;

	return TRUE;
}