#include <typedefines.h>
#include <stdarg.h>
#include <io.h>
#include <systemcalls.h>


#define DEBUG_putch(x) outb(0xE9, x)

int init(void);
void _JustATest(void);
void _AnotherThread(void);
void _PIT_Test(void);
extern void Bochs_printf(const char*, ...);
extern void Bochs_puts(const char*);
extern void Bochs_putch(const char);
extern void* calloc(uint32_t);
extern void initHeap();
extern void free(void*);

int init()
{
	_IOPERM();
	initHeap();
	_TM_SETNAME("TEST");
	_TM_FORK((uint32_t)&_PIT_Test, "PIT");
	_TM_FORK((uint32_t)&_JustATest, "NA");
	_TM_FORK((uint32_t)&_AnotherThread, "JustAnother");
	uint32_t PITid = 0;
	_TM_FINDID(PITid, "PIT");
	while (PITid) {
		// DEBUG_putch('A');
		_IPC_WAITMSG(0);
		char* MSG = (char*) 0xD0000000;
		// while(*MSG)
			// DEBUG_putch(*MSG++);
		Bochs_puts((const char*)MSG);
	}
	return 0;
}

void _JustATest()
{
	while (1) {
		Bochs_printf("Hello %x ", 0x1337FADE);
		_yeild();
	}
}

void _AnotherThread()
{
	// initHeap();
	uint32_t TESTid = 0;
	_TM_FINDID(TESTid, "TEST");
	
	void* DATA = (void*) 0xD0000000;
	char* MSG = (char*) calloc(0x200);
	MSG[0] = '\t';
	MSG[1] = 'J';
	MSG[2] = 'u';
	MSG[3] = 's';
	MSG[4] = 't';
	MSG[5] = 'A';
	MSG[6] = 'T';
	MSG[7] = 'e';
	MSG[8] = 's';
	MSG[9] = 't';
	MSG[10] = '\n';
	MSG[11] = 0;
	while(TESTid) {
		// DEBUG_putch('P');
		

		memcpy(DATA, MSG, 0x20);
		_IPC_SENDMSG(TESTid, 0);
		_yeild();
		// uint32_t x = 0;
		// _TM_FINDID(x, "PIT");
		// DEBUG_putch((char)(((uint8_t) x & 0xFF) + '0'));
	}
}

void _PIT_Test()
{
	// initHeap();
	uint32_t TESTid = 0;
	_TM_FINDID(TESTid, "TEST");
	
	void* DATA = (void*) 0xD0000000;
	char* MSG = (char*) calloc(0x200);
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
	while(TESTid) {
		// DEBUG_putch('P');
		

		memcpy(DATA, MSG, 0x20);
		_IPC_WAITINT(0x20);
		_IPC_SENDMSG(TESTid, 0);
		// _yeild();
		// uint32_t x = 0;
		// _TM_FINDID(x, "PIT");
		// DEBUG_putch((char)(((uint8_t) x & 0xFF) + '0'));
	}
}
