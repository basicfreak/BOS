#include <typedefines.h>
#include <io.h>
#include <systemcalls.h>


#define DEBUG_putch(x) outb(0xE9, x)

int init(void);
void _PIT_Test(void);

int init()
{
	_IOPERM();
	_TM_FORK((uint32_t)&_PIT_Test);
	for(;;) {
		DEBUG_putch('a');
		_yeild();
	}
}

void _PIT_Test()
{
	while(TRUE) {
		_IPC_WAITINT(0x20);
		DEBUG_putch('p');
		// _yeild();
	}
}