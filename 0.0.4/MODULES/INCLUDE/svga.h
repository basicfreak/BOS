#ifndef _SVGA_H_INCLUDED
	#define _SVGA_H_INCLUDED

	#include <api.h>

	void pset(int X, int Y, uint32_t Color);
	void FillRect(int X1, int Y1, int X2, int Y2, uint32_t C);
	void DrawRect(int X1, int Y1, int X2, int Y2, uint32_t C);
	void DrawLine(int X1, int Y1, int X2, int Y2, uint32_t C);
	void ClearScreen(uint32_t C);
	void putch(const char chr, uint32_t Color);
	void puts(const char* str, uint32_t Color);
	void printf(const char* str, ...);
	void cprintf(uint32_t Color, const char* str, ...);
	void refreshScreen(void);

#endif