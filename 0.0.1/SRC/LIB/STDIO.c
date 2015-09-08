/*
./LIBSRC/STDIO.C
*/

#include <STDIO.H>
#include <STRING.H>
#include "../HARDWARE/8042/KEYBOARD.H"

/*
---------------------------------------------------------------------------------------
KEYBOARD INPUT
---------------------------------------------------------------------------------------
*/
char getch(const char *msg)
{
	keyboard_buffer = '\0';
	puts(msg);
	while (keyboard_buffer == '\0')		//If you're waiting here you have ints on anyways... Right?
		__asm__ __volatile__ ("hlt");	//And there is absolutly no reason to use ALL the CPU TIME!!!
	char out = (char) keyboard_buffer;
	putch(out);
	keyboard_buffer = '\0';
	return out;
}

void gets(char out[1024], const char *msg)
{
	puts(msg);
	char myChar = '\0';
	while ((myChar = getch("")) != '\n')
		switch(myChar) {
			case '\b':
				if(arrayRemove(out, 1))
					puts(" \b");
				break;
			default:
				if (arrayAppend(out, myChar)) {
					movcur(curX, curY);
				}
		}
}

/*
---------------------------------------------------------------------------------------
VIDEO
---------------------------------------------------------------------------------------
*/

void initVideo()
{
	color = 0x07;
	curX = 0;
	curY = 0;
	COLS = 80;
	ROWS = 25;
	movcur_disable = 0;
	cls();
}

uint8_t getColorOf(uint8_t X, uint8_t Y)
{
	uint8_t *VIDMEM = (uint8_t *) 0xb8000;
	return VIDMEM[(((Y * COLS) + X) * 2) + 1];
}
uint8_t getCharOf(uint8_t X, uint8_t Y)
{
	uint8_t *VIDMEM = (uint8_t *) 0xb8000;
	return VIDMEM[((Y * COLS) + X) * 2];
}

void puts(const char *message)
{
	while (*message!=0)
		putch (*message++);
	movcur (curX, curY);
}

void printf(const char *message, ...)
{
	va_list ap;
	va_start(ap, message);
	char msg[2000];
	sf(msg, message, ap);
	puts(msg);
	movcur(curX, curY);
}
void printfc(unsigned int cSet, const char *message, ...) {
	unsigned int tempColor = getColor();
	setColor(cSet);
	va_list ap;
	va_start(ap, message);
	char msg[2000];
	sf(msg, message, ap);
	puts(msg);
	movcur(curX, curY);
	setColor(tempColor);
}

void putch(const char charactor)
{
/*
\b	backspace
\t	horizontal tab
\n	newline
\v	vertical tab
\f	form-feed
\r	carriage return
\\	\
\?	?
\'	'
\"	"
*/
	
	char *VIDMEM = (char *) 0xb8000;
	unsigned int memLoc = ((curY * COLS) + curX) *2;
	int t;
	switch ( charactor )
	{
		case '\n':
			curX = 0;
			curY ++;
			break;
		case '\r':
			curX = 0;
			break;
		case '\b':
			curX --;
			break;
		case '\t':
			t = 0;
			(((curX % 5) != 5) ? (t += (5 - (curX % 5))) : (t += 5) );
			for(int i = 0; i < t; i++){
				VIDMEM[ memLoc + (t*2)] = (char) ' ';
				VIDMEM[ memLoc + (t*2) + 1 ] = (char) color;
				curX ++;
			}
			break;
		case '\v':
			curY += 4;
			break;
		case '\f':
			curY ++;
			break;
		case '\\':
			VIDMEM[ memLoc ] = (char) 0x5C;
			VIDMEM[ memLoc + 1 ] = (char) color;
			curX ++;
			break;
		case '\?':
			VIDMEM[ memLoc ] = (char) 0x3F;
			VIDMEM[ memLoc + 1 ] = (char) color;
			curX ++;
			break;
		case '\'':
			VIDMEM[ memLoc ] = (char) 0x27;
			VIDMEM[ memLoc + 1 ] = (char) color;
			curX ++;
			break;
		case '\"':
			VIDMEM[ memLoc ] = (char) 0x22;
			VIDMEM[ memLoc + 1 ] = (char) color;
			curX ++;
			break;
		
		default:
			VIDMEM[ memLoc ] = charactor;
			VIDMEM[ memLoc + 1 ] = (char) color;
			curX ++;
			break;
	}
	if (!scroll_disable) {
		if (curX >= COLS) {
			curX = 0;
			curY ++;
		}
		while (curY >= (ROWS-2))
			scrollScreen ();
	}
}
void movcur(unsigned int x, unsigned int y)
{
	if ( movcur_disable == 0 )
	{
		unsigned short position = (unsigned short) ((y*COLS) + x);
		outb(0x3D4, 0x0F);
		outb(0x3D5, (unsigned char)(position&0xFF));
		outb(0x3D4, 0x0E);
		outb(0x3D5, (unsigned char )((position>>8)&0xFF));
		curX = x;
		curY = y;
	}
}

void setPos(unsigned int x, unsigned int y)
{
	curX = x;
	curY = y;
}

void getPos(unsigned int *x, unsigned int *y)
{
	*x = curX;
	*y = curY;
}

void scrollScreen()
{
	unsigned short *VIDMEM = (unsigned short *) 0xb8000;
	unsigned int i;
	/*for (i=0; i < (ROWS-3)*COLS; i++) //-1
		VIDMEM[i] = VIDMEM[i+80];*/
	memcpy(VIDMEM, VIDMEM+80, (size_t) ((ROWS-3)*COLS*2));
	for (i=(ROWS-3)*COLS; i < (ROWS-2)*COLS; i++) //0
		VIDMEM[i] = (unsigned short) (0x20 | (color << 8));
	curY--;
}

void cls()
{
	unsigned short blank;
    unsigned int i;
	unsigned short *VIDMEM = (unsigned short *) 0xb8000;
	blank = (unsigned short)(0x20 | (color << 8));
	for (i = 0; i < ROWS; i++)
        memsetw ((unsigned short *)(VIDMEM + i * COLS), blank, (size_t) COLS);
	curX = 0;
	curY = 0;
    movcur (0, 0);
}

void setColor(unsigned int cSet)
{
	color = cSet;
}

unsigned int getColor()
{
	return color;
}



/*
---------------------------------------------------------------------------------------
STANDARD IO
---------------------------------------------------------------------------------------
*/

uint8_t inb (uint16_t _port)
{
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outb (uint16_t _port, uint8_t _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

uint16_t inw (uint16_t _port)
{
    uint16_t rv;
    __asm__ __volatile__ ("inw %1, %0" : "=aN" (rv) : "dN" (_port));
    return rv;
}

void outw (uint16_t _port, uint16_t _data)
{
    __asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "aN" (_data));
}

uint32_t inl (uint16_t _port)
{
    uint32_t rv;
    __asm__ __volatile__ ("inl %1, %0" : "=aL" (rv) : "dN" (_port));
    return rv;
}

void outl (uint16_t _port, uint32_t _data)
{
    __asm__ __volatile__ ("outl %1, %0" : : "dN" (_port), "aL" (_data));
}

void *memcpy(void *dest, const void *src, size_t count)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for (; count != 0; count--)
		*dp++ = *sp++;
	return dest;
}

void *memset(void *dest, unsigned char val, size_t count)
{
	unsigned char *temp = (unsigned char *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
	return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	unsigned short *temp = (unsigned short *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
	return dest;
}