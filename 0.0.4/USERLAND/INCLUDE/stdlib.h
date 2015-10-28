#ifndef _STDLIB_H_INCLUDE
	#define _STDLIB_H_INCLUDE

	#include <typedefines.h>

	void Bochs_putch(const char);
	void Bochs_puts(const char*);
	void Bochs_printf(const char*, ...);
	void memset(void*, uint8_t, uint32_t);
	void memsetw(void*, uint16_t, uint32_t);
	void memsetd(void*, uint32_t, uint32_t);
	void itoa(uint32_t, uint32_t, uint8_t*);
	void itoa_s(uint32_t, uint32_t, uint8_t*);

#endif