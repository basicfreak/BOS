#include <PIT.h>
#include <8259.h>

#define TICK_FREQ 10			// 2 MINIMUM - 596590 MAXIMUM (2Hz - ~596.5MHz)

								//Virtual 128-BIT Variable for Timer Ticks
								//128-BIT MAX = 340,282,366,920,938,463,463,374,607,431,768,211,456
uint32_t _PIT_Ticks;			//Ticks Since Startup, Will loop to 0 at 2^32
uint32_t _PIT_Loops;			//Times ticks reset to 0, Will loop to 0 at 2^32
uint32_t _PIT_Loops1;			//Times ticks reset to 0, Will loop to 0 at 2^32
uint32_t _PIT_Loops2;			//Times ticks reset to 0, Will loop to 0 at 2^32

uint8_t _PIT_Seconds;			//6-BIT MAX = 63 we need 60 MAX
uint8_t _PIT_Minutes;
uint8_t _PIT_Hours;				//5-BIT MAX = 31 we need 24 MAX
uint16_t _PIT_Days;				//9-BIT MAX = 511 we need 365 MAX
uint32_t _PIT_Years;			//64-BIT MAX = 18,446,744,073,709,551,615

void PIT_TASK_IR(regs *r);

void PIT_TASK_IR(regs *r)
{
	_PIT_Ticks++;
	if(_PIT_Ticks % TICK_FREQ)
		_PIT_Seconds++;
	if(_PIT_Seconds >= 60) {
		_PIT_Minutes++;
		_PIT_Seconds = (uint8_t) (_PIT_Seconds - 60);
	}
	if(_PIT_Minutes >= 60) {
		_PIT_Hours++;
		_PIT_Minutes = (uint8_t) (_PIT_Minutes - 60);
	}
	if(_PIT_Hours >= 24) {
		_PIT_Days++;
		_PIT_Hours = (uint8_t) (_PIT_Hours - 24);
	}
	if(_PIT_Days >= 365) {
		_PIT_Years++;
		_PIT_Days = (uint16_t) (_PIT_Days - 365);
	}

	if(_PIT_Ticks == 0) {
		_PIT_Loops++;
		if(_PIT_Loops == 0) {
			_PIT_Loops1++;
			if(_PIT_Loops1 == 0)
				_PIT_Loops2++;
		}
	}
}

void PIT_wait(uint32_t T)
{
	uint32_t done = (_PIT_Ticks + T);
	while(_PIT_Ticks < done)
		nop();
}

void sleep(uint32_t S)
{
	while(S--)
		PIT_wait(TICK_FREQ);
}

void _PIT_init(void)
{
	_PIT_Ticks = 0;
	_PIT_Loops = 0;
	_PIT_Seconds = 0;
	_PIT_Minutes = 0;
	_PIT_Hours = 0;
	_PIT_Days = 0;
	_PIT_Years = 0;
	// Set To send IRQ at 100.007Hz 11931     int divisor = 1193180 / hz;
	uint16_t divisor = (1193180 / TICK_FREQ);
	outb(0x43, 0x36);
	outb(0x40, (uint8_t) (divisor & 0xFF));
	outb(0x40, (uint8_t) (divisor >> 4));
	newHandler(PIT_TASK_IR, 0x20);
    /* Unmask IRQ0 */
    _8259_Enable_IRQ(0);
}

uint32_t PIT_Ticks(void)
{
	return _PIT_Ticks;
}

uint32_t PIT_Loops(void)
{
	return _PIT_Loops;
}

uint32_t PIT_Loops1(void)
{
	return _PIT_Loops1;
}

uint32_t PIT_Loops2(void)
{
	return _PIT_Loops2;
}
