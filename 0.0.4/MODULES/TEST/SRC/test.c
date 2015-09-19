#include <io.h>
#include <systemcalls.h>


#define DEBUG_putch(x) outb(0xE9, x)

int init(void);

int init()
{
	_IOPERM();
	for(;;) {
		DEBUG_putch('a');
		_yeild();
	}
}