/*
./DRIVERSRC/SYSTEM/API/THREADMAN.H
*/

#ifndef				THREADMAN_H_INCLUDED
	#define				THREADMAN_H_INCLUDED
	
	#define MAX_THREADS 100
	
	#include <FORMATTING.H>
	#include "../MEM/VIRTUAL.H"
	
	typedef struct ThreadRegs	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t eip, cs, eflags, useresp, ss;
	} tregs;
	
	typedef struct Process{
		tregs registers;
		pdir_t *cr3;
		uint8_t flags;
		uint32_t physaddr;
	} thread_t;
	
	thread_t THREAD[MAX_THREADS];
	bool _THREADMAN_ACTIVE_;
	extern void _THREAD_MAN_PIT_ENTRY(tregs *r);
	void _THREAD_MAN_init(void);
	void KillThread(uint16_t Thread);
	uint16_t AddThread(uint32_t location, uint8_t flags, bool user);
	void _THREAD_MAN_enable(void);
	void _THREAD_MAN_disable(void);
	
#endif