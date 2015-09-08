/*
./LIBSRC/MEM/PAGEFAULT.C
*/

#include <PAGEFAULT.H>
#include <IDT.h>

void pageFault_handler(regs *r);

extern void KillCurrentThreadISRs(regs *r);

/**
US RW  P - Description
0  0  0 - Supervisory process tried to read a non-present page entry
0  0  1 - Supervisory process tried to read a page and caused a protection fault
0  1  0 - Supervisory process tried to write to a non-present page entry
0  1  1 - Supervisory process tried to write a page and caused a protection fault
1  0  0 - User process tried to read a non-present page entry
1  0  1 - User process tried to read a page and caused a protection fault
1  1  0 - User process tried to write to a non-present page entry
1  1  1 - User process tried to write a page and caused a protection fault
**/

const char *PFException[] =
{
    "Supervisory process tried to read a non-present page entry",
	"Supervisory process tried to read a page and caused a protection fault",
	"Supervisory process tried to write to a non-present page entry",
	"Supervisory process tried to write a page and caused a protection fault",
	"User process tried to read a non-present page entry",
	"User process tried to read a page and caused a protection fault",
	"User process tried to write to a non-present page entry",
	"User process tried to write a page and caused a protection fault"
};

void pageFault_handler(regs *r)
{
	DEBUG_printf("Page Fault!\n%s\n", PFException[(r->err_code) & 0x7]);
	DEBUG_printf("\nEAX = 0x%x\tEBX = 0x%x\tECX = 0x%x\tEDX = 0x%x\n", r->eax, r->ebx, r->ecx, r->edx);
	DEBUG_printf("EDI = 0x%x\tESI = 0x%x\tEBP = 0x%x\tESP = 0x%x\n", r->edi, r->esi, r->ebp, r->esp);
	DEBUG_printf("GS = 0x%x\tFS = 0x%x\tES = 0x%x\tDS = 0x%x\n", r->gs, r->fs, r->es, r->ds);
	DEBUG_printf("EIP = 0x%x\tCS = 0x%x\tEFLAGS = 0x%x\tSS = 0x%x\n", r->eip, r->cs, r->eflags, r->ss);
	DEBUG_printf("USER-ESP = 0x%x\tERROR-CODE = 0x%x\n", r->useresp, r->err_code);
	
	uint32_t _cr0 = 0;
	uint32_t _cr2 = 0;
	uint32_t _cr3 = 0;
	uint32_t _cr4 = 0;
	__asm__ __volatile__ ("mov %%cr0, %0":"=b"(_cr0));
	__asm__ __volatile__ ("mov %%cr2, %0":"=b"(_cr2));
	__asm__ __volatile__ ("mov %%cr3, %0":"=b"(_cr3));
	__asm__ __volatile__ ("mov %%cr4, %0":"=b"(_cr4));
	
	DEBUG_printf("CR0 = 0x%x\tCR2 = 0x%x\tCR3 = 0x%x\tCR4 = 0x%x\n", _cr0, _cr2, _cr3, _cr4);
	
	if (r->cs == 0x08) {		//KERNEL LEVEL
		DEBUG_printf("\t\t\t\t\t\t\t\t System  Halted\n");
		__asm__ __volatile__ ("cli");
		__asm__ __volatile__ ("hlt");
		for (;;);
	} else {
		DEBUG_printf("\t\t\t\t\t\t\t\t Attempting Restoration\n");
		//KillCurrentThreadISRs(r);
	}
}

void _pageFault_init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.2\t%s\tCompiled at %s on %s\tMAIN\t  Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	newHandler(pageFault_handler, 14);
}