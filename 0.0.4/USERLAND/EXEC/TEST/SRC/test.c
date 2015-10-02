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
extern void* malloc(uint32_t);
extern void initHeap();
extern void free(void*);

extern uint32_t PCI_findByClass(uint8_t, uint8_t);
extern void PCI_getConfig(void*, uint32_t);

int init()
{
	initHeap();
	_IOPERM();
	_TM_SETNAME("TEST");
	_TM_FORK((uint32_t)&_PIT_Test, "PIT");
	_TM_FORK((uint32_t)&_AnotherThread, "JustAnother");
	_TM_FORK((uint32_t)&_JustATest, "NA");
	while (1) {
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
	void* Buffer = calloc(0x1400);
	uint32_t addr = 0;
	while (1) {
		Bochs_printf("Hello %x ", 0x1337FADE);
		_yeild();
		for(uint8_t C = 0; C < 0x11; C++) {
			for(uint8_t S = 0; S < 0x0F; S++) {
				if((addr = PCI_findByClass(C, S)) != 0xFFFFFFFF) {
					PCI_getConfig(Buffer, addr);
					Bochs_printf("\nClass 0x%x\tSubClass 0x%x:\n", C, S);
					for(int x = 0; x < 0x40; x++)
						Bochs_printf("%x ", ((uint32_t*)Buffer)[x]);
					Bochs_putch('\n');
				}
			}
		}
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
