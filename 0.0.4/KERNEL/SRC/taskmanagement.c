#include <taskmanagement.h>
#include <i386/memory/physical.h>
#include <i386/memory/virtual.h>
#include <i386/cpu/i686.h>

//User Land References:
#define MSG_BASE	0xDF000000
#define CODE_BASE	0x01000000 //16MB
#define DATA_BASE	0x41000000
#define BSS_BASE	0x61000000
#define STACK_TOP	0xCFFFFFFF
#define STACK_BOT	0xCF800000

typedef struct INTTasks {
	uint32_t TaskID[256];
}__attribute__ ((packed)) INTTSK_t, *INTTSK_p; // 1 KB

#define MAX_THREADS 4096
#define MAX_MSGS 2048

uint32_t CurrentThread;
uint32_t LastThread;


#define TF_ACTIVE	0x00000001

#define TF_WAITINT	0x00000004
#define TF_WAITMSG	0x00000008
#define TF_USESSE	0x00000010

#define TF_DEAD		0xFFFFFFFE
#define TF_REGS		0x00000080 // Use this and EAX, EBX, ECX, EDX, ESI, EDI will be saved from the previous thread when called by int handler

#define TF_LINKF	0xFFFFFC00 // if we are waiting for another thread the tread number needs to be stored here this is 22 bits so max threads can be 3FFFFF (4194303)

#define TF_BLANK	0xFF00FF00

extern void IDTBase(void);

extern void GDTBase(void);

typedef struct ThreadEntry {
	regs TRegs; // 76 Bytes
	uint32_t Flags;
	uint32_t CR3;
	char Name[428];
	uint8_t SSEData[512]; // 512 Bytes
}__attribute__((packed)) Thread_t, *Thread_p; // 1KB

typedef struct ThreadList {
	Thread_t Thread[MAX_THREADS];
}__attribute__((packed)) ThreadMan_t, *ThreadMan_p; // 4MB

typedef struct MSGEntry {
	uint32_t SrcID;
	uint32_t DestID;
	uint32_t DataPages;
	void* Data;
}__attribute__((packed)) MSG_t, *MSG_p; // 32 Bytes

typedef struct MSGList {
	MSG_t MSG[MAX_MSGS];
}__attribute__((packed)) MSGMan_t, *MSGMan_p; // 64KB

ThreadMan_p MyThreads;
MSGMan_p MyMSGs;
INTTSK_p MyINTTasks;

void* MSGHeap;

// NOTE: the following trash all registers
// ONLY ACTION AFTER CALLING IS RETURN!!!!
extern void ThreadManager(regs *r);

// void TM_Handler(regs *r);
// void MEM_Handler(regs *r);
// void IPC_Handler(regs *r);
// void API_Handler(regs *r);
uint32_t ReserveEmptyThread(void);
void ForkThread(regs *r);
void ExecThread(regs *r);

bool SendMSG(regs *r, uint32_t DestID, uint32_t MSGSize);

void _TM_init(BootInfo_p BOOTINF)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	MyThreads = (ThreadMan_p) 0xE0001000;
	MyMSGs = (MSGMan_p) 0xE0401000;
	MyINTTasks = (INTTSK_p) 0xE0000000;
	void* ThreadLoc = _PMM_alloc(0x400000);
	void* MSGLoc = _PMM_alloc(0x10000);
	void* INTLoc = _PMM_alloc(0x1000);
	_VMM_map((void*)0xE0000000, INTLoc, FALSE, TRUE);
	for(uint32_t x = 0; x < 0x400000; x += 0x1000)
		_VMM_map((void*) (0xE0001000 + x), (void*)(((uint32_t)ThreadLoc) + x), FALSE, TRUE);
	for(uint32_t x = 0; x < 0x10000; x += 0x1000)
		_VMM_map((void*) (0xE0401000 + x), (void*)(((uint32_t)MSGLoc) + x), FALSE, TRUE);
	memsetd((void*)0xE0000000, 0, 0x0000104400);
	for(uint32_t x = 0; x < MAX_THREADS; x++)
		MyThreads->Thread[x].Flags = TF_BLANK;
	CurrentThread = 0;
	LastThread = 0;
	MSGHeap = (void*)MSG_BASE;
	strcpy(MyThreads->Thread[0].Name, "IDLE");
	MyThreads->Thread[0].Flags = 0;
	MyThreads->Thread[0].CR3 = 0;
	strcpy(MyThreads->Thread[1].Name, "STARTUP");
	MyThreads->Thread[1].Flags = TF_ACTIVE;
	MyThreads->Thread[1].CR3 = getPageDir();
	// MyThreads->Thread[1].TRegs.gs = 0x10;
	// MyThreads->Thread[1].TRegs.fs = 0x10;
	// MyThreads->Thread[1].TRegs.es = 0x10;
	// MyThreads->Thread[1].TRegs.ds = 0x10;
	MyThreads->Thread[1].TRegs.gs = 0x23;
	MyThreads->Thread[1].TRegs.fs = 0x23;
	MyThreads->Thread[1].TRegs.es = 0x23;
	MyThreads->Thread[1].TRegs.ds = 0x23;
	MyThreads->Thread[1].TRegs.eip = 0x01000000;
	// MyThreads->Thread[1].TRegs.cs = 0x8;
	// MyThreads->Thread[1].TRegs.ss = 0x10;
	MyThreads->Thread[1].TRegs.cs = 0x1B;
	MyThreads->Thread[1].TRegs.ss = 0x23;
	MyThreads->Thread[1].TRegs.useresp = 0xCFFFFFFF;
	MyThreads->Thread[1].TRegs.eflags = 0x00000200;
	MyThreads->Thread[1].TRegs.esp = 0xFFB7FFFF;

	MyThreads->Thread[1].TRegs.ebx = (uint32_t) BOOTINF; // This is a special thread ;)
}


bool SendMSG(regs *r, uint32_t DestID, uint32_t MSGSize)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(MSGSize > 0x10000000)
		return FALSE; //ERROR
	if((MyThreads->Thread[DestID].Flags & TF_WAITMSG) &&
		(((MyThreads->Thread[DestID].Flags >> 10) == CurrentThread) || (!(MyThreads->Thread[DestID].Flags >> 10)))) {
		
		uint32_t Pages = (MSGSize / 0x400000);
		if(MSGSize % 0x400000)
			Pages++;

		void* TempPageAddress[Pages];

		for(uint32_t x = 0; x < Pages; x++) {
			TempPageAddress[x] = _VMM_getTable((void*)(0xD0000000 + (x * 0x400000)));
			_VMM_umapTable((void*)(0xD0000000 + (x * 0x400000)));
		}
		// LoadThread(r, DestID);
		for(uint32_t x = 0; x < Pages; x++)
			_VMM_setTable((void*)(0xD0000000 + (x * 0x400000)), TempPageAddress[x], TRUE, FALSE);
		return TRUE;
	}
	return FALSE;
}

uint32_t ReserveEmptyThread()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	for(uint32_t x = 0; x < MAX_THREADS; x++)
		if(MyThreads->Thread[x].Flags == TF_DEAD || MyThreads->Thread[x].Flags == TF_BLANK) {
			MyThreads->Thread[x].Flags = 0;
			return x;
		}
	return 0;
}

void ForkThread(regs *r)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t NewThreadID;
	if((NewThreadID = ReserveEmptyThread())) {
		uint32_t NewPageDir = (uint32_t) _VMM_copyPDir();
		memcpy((void*)((uint32_t)(&MyThreads->Thread[NewThreadID].SSEData)), (void*)((uint32_t)(&MyThreads->Thread[CurrentThread].SSEData)), 512);
		memcpy((void*)((uint32_t)(&MyThreads->Thread[NewThreadID].TRegs)), (void*)r, sizeof(regs));
		MyThreads->Thread[NewThreadID].TRegs.eip = r->edx;
		MyThreads->Thread[NewThreadID].CR3 = NewPageDir;
		MyThreads->Thread[NewThreadID].Flags = MyThreads->Thread[CurrentThread].Flags;

#ifdef DEBUG_EXTREAM
	DEBUG_printf("New Thread Added. CR3 = %x, EIP = %x\n", MyThreads->Thread[NewThreadID].CR3, MyThreads->Thread[NewThreadID].TRegs.eip);
#endif
		// _VMM_setCOWOther((void*)NewPageDir);
		// LoadThread(r, NewThreadID);
	}
}

void ExecThread(regs *r)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.4\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t NewThreadID;
	if((NewThreadID = ReserveEmptyThread())) {
		MyThreads->Thread[NewThreadID].CR3 = r->edx;
		MyThreads->Thread[NewThreadID].TRegs.eip = r->ebx;
		MyThreads->Thread[NewThreadID].TRegs.gs = 0x23;
		MyThreads->Thread[NewThreadID].TRegs.fs = 0x23;
		MyThreads->Thread[NewThreadID].TRegs.es = 0x23;
		MyThreads->Thread[NewThreadID].TRegs.ds = 0x23;
		MyThreads->Thread[NewThreadID].TRegs.cs = 0x1B;
		MyThreads->Thread[NewThreadID].TRegs.ss = 0x23;
		MyThreads->Thread[NewThreadID].TRegs.useresp = 0xCFFFFFFF;
		MyThreads->Thread[NewThreadID].TRegs.eflags = 0x00000200;
		MyThreads->Thread[NewThreadID].TRegs.esp = 0xFFB7FFFF;
		// strcpy(MyThreads->Thread[NewThreadID].Name, (const char*) r->esi);
#ifdef DEBUG
	DEBUG_printf("New Thread Added. CR3 = %x, EIP = %x, NAME = \"%s\"\n", MyThreads->Thread[NewThreadID].CR3,
		MyThreads->Thread[NewThreadID].TRegs.eip, (MyThreads->Thread[NewThreadID].Name));
#endif
		MyThreads->Thread[NewThreadID].Flags = TF_ACTIVE;
		// LoadThread(r, NewThreadID);
	}
}
