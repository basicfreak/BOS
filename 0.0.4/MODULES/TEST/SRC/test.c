#include <typedefines.h>
#include <io.h>
#include <systemcalls.h>


#define DEBUG_putch(x) outb(0xE9, x)

int init(void);
void _JustATest(void);
void _PIT_Test(void);

int init()
{
	_IOPERM();
	_TM_SETNAME("TEST");
	_TM_FORK((uint32_t)&_PIT_Test, "PIT");
	_TM_FORK((uint32_t)&_JustATest, "NA");
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
	while (1) {
		DEBUG_putch('T');
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
