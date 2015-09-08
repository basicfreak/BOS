/*
./INCLUDE/STDIO.H
*/

#include <FORMATTING.H>
#include <stdarg.h>

#ifndef				STDIO_H_INCLUDED
	#define				STDIO_H_INCLUDED

	unsigned int curX;
	unsigned int curY;
	unsigned int COLS;
	unsigned int ROWS;
	unsigned int color;
	unsigned int movcur_disable;
	unsigned int scroll_disable;
	
	char getch(const char *msg);
	void gets(char out[1024], const char *msg);
	
	void initVideo(void);
	uint8_t getColorOf(uint8_t x, uint8_t Y);
	uint8_t getCharOf(uint8_t x, uint8_t Y);
	void puts(const char *message);
	void printf(const char *message, ...);
	void printfc(unsigned int cSet, const char *message, ...);
	void putch(const char charactor);
	void movcur(unsigned int x, unsigned int y);
	void setPos(unsigned int x, unsigned int y);
	void getPos(unsigned int *x, unsigned int *y);
	void scrollScreen(void);
	void cls(void);
	void setColor(unsigned int cSet);
	unsigned int getColor(void);
	extern void txf(uint8_t COM, const char *message, ...);
	
	
	uint8_t inb (uint16_t _port);
	void outb (uint16_t _port, uint8_t _data);
	uint16_t inw (uint16_t _port);
	void outw (uint16_t _port, uint16_t _data);
	uint32_t inl (uint16_t _port);
	void outl (uint16_t _port, uint32_t _data);
	void *memcpy(void *dest, const void *src, size_t count);
	void *memset(void *dest, unsigned char val, size_t count);
	unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
	
	
#endif
