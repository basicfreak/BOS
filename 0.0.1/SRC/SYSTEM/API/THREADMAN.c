/*
./DRIVERSRC/SYSTEM/API/THREADMAN.C
*/

#include "THREADMAN.H"
#include <STDIO.H>
#include "../CPU/IRQ.H"

void KillCurrentThreadISRs(regs *r);

#define DEBUG

uint16_t Current_Thread = 0xFFFF; // 0 always refers to idle thread - 1 will be kernel all info will be saved on first task switch (hopefully...)

extern void _SYSTEM_IDLE_THREAD(void);	// Thread 0
extern void _init2(void);				// Stage 2 of kernel
bool forceidleonecycle = false;

void _THREAD_MAN_PIT_ENTRY(tregs *r)
{
	// Save current Threads Register Information
	if(Current_Thread != 0xFFFF)
		memcpy(&THREAD[Current_Thread].registers, r, sizeof(tregs));
	// Find next thread
	uint16_t Next_Thread;
	/*if(THREAD[2].flags != 0x01) {
		if (Current_Thread) Next_Thread = 0;
		else Next_Thread = 1;
	} else {
		if (Current_Thread == 0) Next_Thread = 1;
		else if (Current_Thread == 1) Next_Thread = 2;
		else Next_Thread = 0;
	}/*/
	for (Next_Thread = (uint16_t) (Current_Thread + 1); THREAD[Next_Thread].flags != 0x01; Next_Thread++)
		if (Next_Thread >= MAX_THREADS)
			Next_Thread = 0;
	if (Next_Thread == Current_Thread)
		Next_Thread = 0;/*
#ifdef DEBUG
	txf(1, "\n\r(THREADMAN.C:Line 43) Switching from Thread 0x%x to 0x%x\n\r", Current_Thread, Next_Thread);
#endif*/
	// Load next Thread
	Current_Thread = Next_Thread;
	__asm__ __volatile__ ("mov %0, %%cr3":: "b"(THREAD[Current_Thread].cr3));
	memcpy(r, &THREAD[Current_Thread].registers, sizeof(tregs));
}

void _THREAD_MAN_init()
{
#ifdef DEBUG
	txf(1, "\n\r(THREADMAN.C:Line 46) _THREAD_MAN_init()\n\r");
#endif
	memset (&THREAD, 0, sizeof(tregs) * MAX_THREADS); // Clear Thread Table
	AddThread((uint32_t) &_SYSTEM_IDLE_THREAD, 0xFF, 0);
	AddThread((uint32_t) &_init2, 0x01, 0);
	_THREAD_MAN_enable();
}

void _THREAD_MAN_enable()
{
#ifdef DEBUG
	txf(1, "\n\r(THREADMAN.C:Line 60) _THREAD_MAN_enable()\n\r");
#endif
	_THREADMAN_ACTIVE_ = true;
}

void _THREAD_MAN_disable()
{
#ifdef DEBUG
	txf(1, "\n\r(THREADMAN.C:Line 68) _THREAD_MAN_disable()\n\r");
#endif
	_THREADMAN_ACTIVE_ = false;
}

extern uint32_t Kernel_Stack(void);

void KillCurrentThreadISRs(regs *r)
{
#ifdef DEBUG
	txf(1, "\n\r(THREADMAN.C:Line 78) KillCurrentThreadISRs(0x%x)\n\r", (uint32_t) r);
	txf(1, "\tKilling Thread 0x%x\n\r", Current_Thread);
#endif
	THREAD[Current_Thread].flags = 0;
	THREAD[Current_Thread].cr3 = 0;
	Current_Thread = 0;
	__asm__ __volatile__ ("mov %0, %%cr3":: "b"(THREAD[Current_Thread].cr3));
	r->gs = THREAD[Current_Thread].registers.gs;
	r->fs = THREAD[Current_Thread].registers.fs;
	r->es = THREAD[Current_Thread].registers.es;
	r->ds = THREAD[Current_Thread].registers.ds;
	
	r->edi = THREAD[Current_Thread].registers.edi;
	r->esi = THREAD[Current_Thread].registers.esi;
	r->ebp = THREAD[Current_Thread].registers.ebp;
	r->esp = THREAD[Current_Thread].registers.esp;
	r->ebx = THREAD[Current_Thread].registers.ebx;
	r->edx = THREAD[Current_Thread].registers.edx;
	r->ecx = THREAD[Current_Thread].registers.ecx;
	r->eax = THREAD[Current_Thread].registers.eax;
	
	r->eip = THREAD[Current_Thread].registers.eip;
	r->cs = THREAD[Current_Thread].registers.cs;
	r->eflags = THREAD[Current_Thread].registers.eflags;
	r->useresp = THREAD[Current_Thread].registers.useresp;
	r->ss = THREAD[Current_Thread].registers.ss;
}

void KillThread(uint16_t Thread)
{
#ifdef DEBUG
	txf(1, "\n\r(THREADMAN.C:Line 109) KillThread(0x%x)\n\r", Thread);
	txf(1, "\tKilling Thread 0x%x\n\r", Thread);
#endif
	THREAD[Thread].flags = 0;
	THREAD[Thread].cr3 = 0;
}

uint16_t AddThread(uint32_t location, uint8_t flags, bool user)
{
#ifdef DEBUG
	txf(1, "\n\r(THREADMAN.C:Line 119) AddThread(0x%x, 0x%x, 0x%x) = ", location, flags, user);
#endif
	bool found = false;
	uint16_t task = 0;
	for (uint16_t i = 0; i < MAX_THREADS ; i++)
		if (!THREAD[i].cr3 && !found) {
			found = true;
			task = i;
		}
#ifdef DEBUG
	txf(1, "%i ", task);
#endif
	tregs tempreg;
	memset(&tempreg, 0, sizeof(tregs));
	if (!user) {		// Kernel Thread
		tempreg.gs = 0x10;
		tempreg.fs = 0x10;
		tempreg.es = 0x10;
		tempreg.ds = 0x10;
		tempreg.ss = 0x10;
		tempreg.cs = 0x08;

		//tempreg.useresp = Kernel_Stack();

		uint8_t *NEWSTACK = calloc(4);
		tempreg.useresp = (uint32_t) &NEWSTACK[0x2FFF];
		
		THREAD[task].cr3 = Kernel_Page_Dir_Address();
		THREAD[task].physaddr = 0x100000;
	} else {			// User Thread
		tempreg.gs = 0x23;
		tempreg.fs = 0x23;
		tempreg.es = 0x23;
		tempreg.ds = 0x23;
		tempreg.ss = 0x23;
		tempreg.cs = 0x1B;
		tempreg.useresp = 0xBFFFFFFF;
		THREAD[task].cr3 = Create_Process_Directory(4096, (uint32_t*) &THREAD[task].physaddr);
	}					// Common
	tempreg.eip = (uint32_t)location;
	tempreg.esp = Kernel_Stack();
	tempreg.eflags = 0x0202;
	memcpy(&THREAD[task].registers, &tempreg, sizeof(tregs));
	THREAD[task].flags = flags;
#ifdef DEBUG
	txf(1, "& EIP = 0x%x\n\r", THREAD[task].registers.eip);
#endif
	return task;
}