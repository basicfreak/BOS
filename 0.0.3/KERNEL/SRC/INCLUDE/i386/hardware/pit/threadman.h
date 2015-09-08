#ifndef				TM_H_INCLUDED
	#define				TM_H_INCLUDED

	#include <KLIB.h>
	#include <i386/cpu/idt.h>

	void _TM_init(void);
	uint32_t mkThread(void *Location, bool User);
	void rmThread(uint32_t Thread);
	void _TM_Pit_Entry(regs *r);

#endif