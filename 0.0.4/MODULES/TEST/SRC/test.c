#include <typedefines.h>
#include <io.h>
#include <systemcalls.h>


#define DEBUG_putch(x) outb(0xE9, x)

int init(void);
void _JustATest(void);
void _AnotherThread(void);
void _PIT_Test(void);
extern void printf(const char* str, ...);

int init()
{
	_IOPERM();
	_TM_SETNAME("TEST");
	_TM_FORK((uint32_t)&_PIT_Test, "PIT");
	_TM_FORK((uint32_t)&_JustATest, "NA");
	_TM_FORK((uint32_t)&_AnotherThread, "JustAnother");
	_yeild(); // Allow Next Task To Start
	uint32_t PITid = 0;
	_TM_FINDID(PITid, "PIT");
	while (PITid) {
		// DEBUG_putch('A');
		_IPC_WAITMSG(PITid);
		char* MSG = (char*) 0xD0000000;
		while(*MSG)
			DEBUG_putch(*MSG++);
	}
	return 0;
}

void _JustATest()
{
	uint8_t* API = (uint8_t*)0x10000;
	// ; ecx = size in bytes
	// ; edi = Where To Make Buffer (in threads virutal space)
	// ; RETURN eax = Actual Virtual Address (API Base Location)
	// ;              So we know where to relocate it to.
	uint32_t ret = 0;
	__asm__ __volatile__ ("xchg %bx, %bx");
	__asm__ __volatile__ ("int $0xF3" : "=a" (ret) : "a" (0xF0), "c" (1024), "D" (0x10000));
	__asm__ __volatile__ ("xchg %bx, %bx");
	API[0] = 0xBA;
	API[1] = 0xE9;
	API[2] = 0x00;
	API[3] = 0x00;
	API[4] = 0x00;
	API[5] = 0xB0;
	API[6] = (uint8_t)'-';
	API[7] = 0xEE;
	API[8] = 0xB0;
	API[9] = (uint8_t)'/';
	API[10] = 0xEE;
	API[11] = 0xC3;
	// void (*apitest)(void) = ret;
	__asm__ __volatile__ ("int $0xF3" : : "a" (0x02), "b" (ret), "S" ("TestAPI"));
	while (1) {
		DEBUG_putch('A');
	// __asm__ __volatile__ ("xchg %bx, %bx");
		// apitest();
		_yeild();
		// printf("Hello\n");
	}
}

void _AnotherThread()
{
	uint32_t ret = 0;
	__asm__ __volatile__ ("int $0xF3" : "=a" (ret) : "a" (0x01), "S" ("TestAPI"));
	void (*apitest)(void) = ret;
	while(1) {
		apitest();
		_yeild();
	}
}

void _PIT_Test()
{
	uint32_t TESTid = 0;
	_TM_FINDID(TESTid, "TEST");
	_yeild();
	while(TESTid) {
		// DEBUG_putch('P');
		char* MSG = (char*) 0xD0000000;
		MSG[0] = '\n';
		MSG[1] = 'P';
		MSG[2] = 'I';
		MSG[3] = 'T';
		MSG[4] = ' ';
		MSG[5] = 'F';
		MSG[6] = 'i';
		MSG[7] = 'r';
		MSG[8] = 'e';
		MSG[9] = 'd';
		MSG[10] = '\n';
		MSG[11] = 0;
		_IPC_WAITINT(0x20);
		_IPC_SENDMSG(TESTid, 0);
		// _yeild();
		// uint32_t x = 0;
		// _TM_FINDID(x, "PIT");
		// DEBUG_putch((char)(((uint8_t) x & 0xFF) + '0'));
	}
}
