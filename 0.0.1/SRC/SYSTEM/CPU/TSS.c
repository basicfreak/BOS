/*
./LIBSRC/TSS.C
*/

#include "TSS.H"
#include "GDT.H"
#include <STDIO.H>

static struct tss_entry TSS;

void flush_tss (uint16_t sel);


void flush_tss (uint16_t sel)
{
	__asm__ __volatile__ ("ltr %0"::"r" (sel));
	/*__asm__ __volatile__ (".intel_syntax;\
		mov ax, %0;\
		ltr ax;\
	.att_syntax;"::"r"(sel):"ax");*/
}

void tss_set_stack (uint32_t kernelSS, uint32_t kernelESP)
{
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
}

uint32_t Kernel_Stack()
{
	return TSS.esp0;
}

void install_tss (uint8_t idx, uint32_t kernelSS, uint32_t kernelESP)
{
	// install TSS descriptor
	uint32_t base = (uint32_t) &TSS;

	// install descriptor
	install_ring(idx, base, base + sizeof (struct tss_entry),
		///_ACCESS | _EXEC_CODE | _DPL | _MEMORY,
		(_MEMORY | _EXEC_CODE | _ACCESS),
		0);

	// initialize TSS
	memset ((void*) &TSS, 0, sizeof (struct tss_entry));

	// set stack and segments
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
	TSS.cs = 0x0b;
	TSS.ss = 0x13;
	TSS.es = 0x13;
	TSS.ds = 0x13;
	TSS.fs = 0x13;
	TSS.gs = 0x13;
	TSS.iomap = sizeof(TSS);

	// flush TSS
	flush_tss((uint16_t) (idx * sizeof (struct gdt_entry)));
}