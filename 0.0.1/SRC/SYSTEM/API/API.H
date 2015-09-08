/*
./DRIVERSRC/SYSTEM/API/API.H
*/

#ifndef				API_H_INCLUDED
	#define				API_H_INCLUDED
	
	#include <FORMATTING.H>
	
	void _API_init(void);
	void INT(uint8_t num, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
	
#endif