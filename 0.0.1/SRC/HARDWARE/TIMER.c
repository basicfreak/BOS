/*
./LIBSRC/TIMER.C
*/

#include "TIMER.H"
#include "../SYSTEM/CPU/8259.H"
#include <STDIO.H>
#include "../SYSTEM/API/THREADMAN.H"

uint32_t GetTimerTicks()
{
	return timer_ticks;
}

void PIT_TASK_IR(tregs *r);

void PIT_TASK_IR(tregs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;
	if(_THREADMAN_ACTIVE_)
		_THREAD_MAN_PIT_ENTRY(r);
	if (timer_ticks % 15 == 0 && UPTIME_ACTIVE) display_uptime(r); //update every 150 ms
}
void display_uptime(tregs *r)
{
	movcur_disable ++;
	scroll_disable ++;
	unsigned int tempX = curX;
	unsigned int tempY = curY;
	curX = 0;
	curY = 23;
	printf("EAX = 0x%x    EBX = 0x%x    ECX = 0x%x    EDX = 0x%x               ", r->eax, r->ebx, r->ecx, r->edx);
	curX = 0;
	curY = 24;
	printf("CS = 0x%x    DS = 0x%x    ESI = 0x%x    EIP = 0x%x    EFLAGS = 0x%x               ", r->cs, r->ds, r->esi, r->eip, r->eflags);
	curX = tempX;
	curY = tempY;
	movcur_disable --;
	scroll_disable --;
	return;
}

void timer_wait(unsigned int ticks)
{
    unsigned long eticks;
    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
	return;
}

void sleep(unsigned int secs)
{
	while(secs--) timer_wait(100);
	return;
}

/* Sets up the system clock by installing the timer handler into IRQ0 */
void _TIMER_init()
{
	UPTIME_ACTIVE = TRUE;
	timer_ticks = 0;
	// Set To send IRQ at 100.007Hz 11931     int divisor = 1193180 / hz;
	outb(0x43, 0x36);
	outb(0x40, 0x9B);
	outb(0x40, 0x2E);
    /* Unmask IRQ0 */
    _8259_Enable_IRQ(0);
}