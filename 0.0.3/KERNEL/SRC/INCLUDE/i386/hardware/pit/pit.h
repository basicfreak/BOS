/*
./SRC/INCLUDE/PIT.H
*/

#ifndef				PIT_H_INCLUDED
	#define				PIT_H_INCLUDED
	
	#include <KLIB.h>
	#include <i386/cpu/idt.h>

	void PIT_wait(uint32_t T);
	void sleep(uint32_t S);
	void _PIT_init(void);
	uint32_t PIT_Ticks(void);
	/*uint32_t PIT_Loops(void);
	uint32_t PIT_Loops1(void);
	uint32_t PIT_Loops2(void);
*/
#endif