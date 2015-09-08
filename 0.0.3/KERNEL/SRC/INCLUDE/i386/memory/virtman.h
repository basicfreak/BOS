#ifndef				VMM_H_INCLUDED
	#define				VMM_H_INCLUDED
	
	#include <KLIB.h>
	#include <BOSBOOT.h>
	
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
		I86_FRAME			=	0x7FFFF000 	//1111111111111111111000000000000
	};

	typedef uint32_t PageEntry;
	typedef struct Page_Dir_Table {
		PageEntry entry[1024];
	} Pageing_t, *Paging_p;

	void _VMM_init(BootInfo_p);
	void _VMM_map(uint32_t Physical, uint32_t Virtual, bool User, bool Write);
	void _VMM_unmap(uint32_t Virtual);
	void *_VMM_malloc(uint32_t Virtual, uint32_t Pages, bool user, bool write);
	void _VMM_free(uint32_t Virtual, uint32_t Pages);
	void *getPhysical(uint32_t Virtual);
	void *mkPageDir(void);
	extern void _VMM_setCR3(uint32_t PDir);

#endif