#include <typedefines.h>
#include <io.h>
#include <systemcalls.h>


#define DEBUG_putch(x) outb(0xE9, x)

int init(void);
void _PIT_Test(void);

int init()
{
	_IOPERM();
	_TM_SETNAME("TEST");
	_TM_FORK((uint32_t)&_PIT_Test);
	_yeild(); // Allow Next Task To Start
	uint32_t PITid = 0;
	while(!PITid)
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

void _PIT_Test()
{
	_TM_SETNAME("PIT");
	uint32_t TESTid = 0;
	while(!TESTid)
		_TM_FINDID(TESTid, "TEST");
	_yeild();
	while(TESTid) {
		_IPC_WAITINT(0x20);
		// DEBUG_putch('P');
		char* MSG = (char*) 0xD0000000;
		MSG[0] = 'P';
		MSG[1] = 'I';
		MSG[2] = 'T';
		MSG[3] = ' ';
		MSG[4] = 'F';
		MSG[5] = 'i';
		MSG[6] = 'r';
		MSG[7] = 'e';
		MSG[8] = 'd';
		MSG[9] = '\n';
		MSG[10] = 0;
		_IPC_SENDMSG(TESTid, 0);
		// _yeild();
		// uint32_t x = 0;
		// _TM_FINDID(x, "PIT");
		// DEBUG_putch((char)(((uint8_t) x & 0xFF) + '0'));
	}
}