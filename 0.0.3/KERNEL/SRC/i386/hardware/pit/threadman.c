#include <i386/hardware/pit/threadman.h>
#include <i386/memory/heapman.h>
#include <i386/memory/virtman.h>
#include <i386/memory/physman.h>

#define _MAX_Threads 1024
uint32_t CurrentThread;

typedef struct ThreadManEntry {
	uint8_t Flags;
	regs TRegs;
	uint32_t PageDir;
} __attribute__((packed)) TMEntry_t, *TMEntry_p;

typedef struct ThreadManList {
	TMEntry_p Entry[_MAX_Threads];
} __attribute__((packed)) TMList_t, *TMList_p;

TMList_p ThreadList;

void _TM_init()
{
	ThreadList = (TMList_p)calloc(0x1000);
	(void) mkThread(0,0);
	newHandler(_TM_Pit_Entry, 0x20);
}
/*typedef struct registers	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;    
	} __attribute__((packed)) regs;*/
uint32_t mkThread(void *Location, bool User)
{
	for(uint32_t x = 0; x < _MAX_Threads; x++)
		if(!ThreadList->Entry[x]) { // Does this not exist?
			ThreadList->Entry[x] = (TMEntry_p) calloc(sizeof(TMEntry_t));
			TMEntry_p TEnt = ThreadList->Entry[x];
			TEnt->PageDir = mkPageDir();
			cli();
			_VMM_setCR3(TEnt->PageDir);
			
			sti();
			return x;
		}
	return 0xFFFFFFFF;
}

void rmThread(uint32_t Thread)
{
	if(ThreadList->Entry[Thread]) {
		free(ThreadList->Entry[Thread], sizeof(regs));
		ThreadList->Entry[Thread] = (TMEntry_p) 0;
	}
}

void _TM_Pit_Entry(regs *r)
{
	//
}
