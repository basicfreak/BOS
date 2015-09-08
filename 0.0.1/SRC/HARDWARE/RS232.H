/*
./DRIVERSRC/RS232.H
*/

#ifndef				_RS232_H_INCLUDED
	#define				_RS232_H_INCLUDED
	
	#include <FORMATTING.H>
	#include <stdarg.h>
	#include <STDIO.H>

	void _RS232_init(void);
	void txc(uint8_t COM, uint8_t data);
	void txs(uint8_t COM, const char *message);
	
#endif