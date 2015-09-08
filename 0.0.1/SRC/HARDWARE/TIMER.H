/*
./INCLUDE/TIMER.H
*/

#include <FORMATTING.H>

#ifndef				TIMER_H_INCLUDED
	#define				TIMER_H_INCLUDED
	
	#include "../SYSTEM/API/THREADMAN.H"
	
	unsigned long timer_ticks;
	unsigned int timer_seconds;
	unsigned int timer_minutes;
	unsigned int timer_hours;
	unsigned int timer_days;
	unsigned int timer_years;
	bool UPTIME_ACTIVE;
	
	void display_uptime(tregs *r);
	void timer_wait(unsigned int ticks);
	void sleep(unsigned int secs);
	void _TIMER_init(void);
	uint32_t GetTimerTicks(void);
	
#endif
