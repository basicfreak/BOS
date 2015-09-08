#include <svga.h>
#include <BOSBOOT.h>

uint16_t Width;
uint16_t Height;

int init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	Width = BOOTINF.CurVBEInfo->Width - 1;
	Height = BOOTINF.CurVBEInfo->Height - 1;
	ClearScreen(0);
	while(1)
	for(uint32_t C = 0 ; C <= 0xFFFFFFF0; C++) {
		ClearScreen(C);
		cprintf(~C, "\n\t0x%x", C);
		printf("\n\n\n\tThis Is A Test Of Printf 0xF213 = 0x%x = %i %s", 0xF213, 0xF213, "Test Completed\n");
		// PIT_wait(10);
		refreshScreen();
	}
	return 0;
}