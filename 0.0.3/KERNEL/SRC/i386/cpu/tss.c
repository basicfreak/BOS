#include <i386/cpu/tss.h>
#include <i386/cpu/gdt.h>

extern void _TSS_flush(void);

typedef struct tss_entry_struct
{
	uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
	uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
	uint32_t ss0;        // The stack segment to load when we change to kernel mode.
	uint32_t esp1;       // everything below here is unusued now.. 
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;         
	uint32_t cs;        
	uint32_t ss;        
	uint32_t ds;        
	uint32_t fs;       
	uint32_t gs;         
	uint32_t ldt;      
	uint16_t trap;
	uint16_t iomap_base; // Size Of TSS_t (104)
} __attribute__((packed)) TSS_t;

TSS_t MyTSS;

void _TSS_init()
{
	memset(&MyTSS, 0, sizeof(TSS_t));
	MyTSS.ss0 = 0x10;
	//asm ("movl %%esp, %0" : "=r" (MyTSS.esp0) );
	_TSS_updateESP();
	uint32_t base = (uint32_t) &MyTSS;
	InstallGDTRing(5, base, (base + (uint32_t) sizeof(TSS_t)), ( _ACCESS | _EXEC_CODE | _DPL | _MEMORY ), 0);
	_TSS_flush();
}

void _TSS_updateESP()
{
	asm ("movl %%esp, %0" : "=r" (MyTSS.esp0) );
}