/*
./SRC/INCLIDE/KLIB.h
*/

#ifndef				KLIB_H_INCLUDED
	#define				KLIB_H_INCLUDED

	#define DEBUG
	#define DEBUG_FULL

	#include <stdarg.h>

// Define basic data types
	typedef char bool;
	typedef int size_t;
	typedef signed char int8_t;
	typedef unsigned char uint8_t;
	typedef short int16_t;
	typedef unsigned short uint16_t;
	typedef int int32_t;
	typedef unsigned uint32_t;
	typedef long long int64_t;
	typedef unsigned long long uint64_t;

	#define NULL 0
	#define TRUE 1
	#define FALSE 0
	#define true 1
	#define false 0

// Define public functions
	#define bit(n) (1 << n)
	#define nop() __asm__ __volatile__("nop")
	#define cli() __asm__ __volatile__("cli")
	#define sti() __asm__ __volatile__("sti")
	#define hlt() __asm__ __volatile__("hlt")
	#define printBOOL(x) ((x) ? "TRUE" : "FALSE")


	void outb (uint16_t port, uint8_t data);
	uint8_t inb (uint16_t port);
	void outw (uint16_t port, uint16_t data);
	uint16_t inw (uint16_t port);
	void outl (uint16_t port, uint32_t data);
	uint32_t inl (uint16_t port);
	void *memcpy (void *dest, const void *src, size_t count);
	void *memset (void *dest, uint8_t val, size_t count);
	uint16_t *memsetw (uint16_t *dest, uint16_t val, size_t count);

	size_t strlen (const uint8_t *str);
	bool streql ( const uint8_t *s1, const uint8_t *s2 );
	int32_t strcmp ( const uint8_t * s1, const uint8_t * s2 );
	uint8_t *strcpy (uint8_t *s1, const uint8_t *s2);
	void itoa (uint32_t i, uint32_t base, uint8_t* buf);
	void itoa_s (uint32_t i, uint32_t base, uint8_t* buf);

	void reboot (void);
	int stringf (char *str, const char *format, va_list ap);

// Linker Variables
	extern void* end(void);
	extern void* sys_stack(void);

	#ifdef DEBUG
		#define DEBUG_PORT 0xE9
		void DEBUG_putch (const char charactor);
		void DEBUG_print (const char *String);
		void DEBUG_printf (const char *String, ...);
	#endif

#endif