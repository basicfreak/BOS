#ifndef				KLIB_H_INCLUDED
	#define				KLIB_H_INCLUDED

	#define DEBUG
	#define DEBUG_FULL
	#define DEBUG_EXTREAM

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

	extern void memcpy(void*, void*, uint32_t);
	extern void memcpyw(void*, void*, uint32_t);
	extern void memcpyd(void*, void*, uint32_t);
	extern void memset(void*, uint8_t, uint32_t);
	extern void memsetw(void*, uint16_t, uint32_t);
	extern void memsetd(void*, uint32_t, uint32_t);
	extern void outb(uint16_t, uint8_t);
	extern uint8_t inb(uint16_t);
	extern void outw(uint16_t, uint16_t);
	extern uint16_t inw(uint16_t);
	extern void outd(uint16_t, uint32_t);
	extern uint32_t ind(uint16_t);

	extern void* end(void);
	extern void* sys_stack(void);
	extern void* KVirtBase(void);

	size_t strlen(const char *str);
	bool streql( const char *s1, const char *s2 );
	int strcmp( const char * s1, const char * s2 );
	char *strcpy(char *s1, const char *s2);

	#ifdef DEBUG
		void DEBUG_printf(const char* Str, ...);
		void DEBUG_print(const char* Str);
		void DEBUG_putch(const char Chr);
		void itoa(uint32_t i, uint32_t base, uint8_t* buf);
		void itoa_s(uint32_t i, uint32_t base, uint8_t* buf);
	#endif

#endif