/*
./SRC/LIB/KLIB.c
*/

#include <KLIB.h>

void outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

uint8_t inb(uint16_t port)
{
	uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void outw(uint16_t port, uint16_t data)
{
	__asm__ __volatile__ ("outw %1, %0" : : "dN" (port), "aN" (data));
}

uint16_t inw(uint16_t port)
{
	uint16_t ret;
    __asm__ __volatile__ ("inw %1, %0" : "=aN" (ret) : "dN" (port));
    return ret;
}

void outl(uint16_t port, uint32_t data)
{
	__asm__ __volatile__ ("outl %1, %0" : : "dN" (port), "aL" (data));
}

uint32_t inl(uint16_t port)
{
	uint32_t ret;
    __asm__ __volatile__ ("inl %1, %0" : "=aL" (ret) : "dN" (port));
    return ret;
}

void *memcpy(void *dest, const void *src, size_t count)
{
	const uint8_t *sp = (const uint8_t *)src;
	uint8_t *dp = (uint8_t *)dest;
	for (; count != 0; count--)
		*dp++ = *sp++;
	return dest;
}

void *memset(void *dest, uint8_t val, size_t count)
{
	uint8_t *temp = (uint8_t *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
	/*count--;
	while(count) {
		// *temp++ = val;
		temp[count] = val;
		count--;
	}
	temp[count] = val;*/
	return dest;
}

uint16_t *memsetw(uint16_t *dest, uint16_t val, size_t count)
{
	uint16_t *temp = (uint16_t *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
	return dest;
}




size_t strlen(const uint8_t *str)
{
	size_t ret = 0;
	while (str[ret++]);
	return ret;
}

bool streql( const uint8_t *s1, const uint8_t *s2 )
{
	int32_t temp = strcmp (s1, s2);
	if (temp == 0) return TRUE;
	return FALSE;
}

int32_t strcmp( const uint8_t * s1, const uint8_t * s2 )
{
	int32_t ret = 0;
	while (!(ret = *(uint8_t*)s1 - *(uint8_t*)s2) && *s2)
    {
        ++s1;
        ++s2;
    }
	if (ret < 0) ret = -1;
	if (ret > 0) ret = 1;
    return	ret;
}

uint8_t *strcpy(uint8_t *s1, const uint8_t *s2)
{
	uint8_t *s1_p = s1;
	while ( (*s1++ = *s2++) );
	return s1_p;
}

uint8_t		tbuf[32];
uint8_t		bchars[] =				{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(uint32_t i, uint32_t base, uint8_t* buf)
{
   uint32_t pos = 0;
   uint32_t opos = 0;
   uint32_t top = 0;

   if (i == 0 || base > 16)
   {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0)
   {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++)
   {
      buf[opos] = tbuf[pos];
   }
   buf[opos] = 0;
   return;
}

void itoa_s(uint32_t i, uint32_t base, uint8_t* buf)
{
   if (base > 16) return;
   itoa(i,base,buf);
   return;
}


void reboot()
{
	uint16_t TimeOutVar = 0;
	cli();
	while((inb(0x64) & 0x02) && (TimeOutVar < 0x9C00)) {
		nop();
		TimeOutVar++;
	}
	TimeOutVar = ((TimeOutVar < 0x9C00) ? (0) : (0xFFF0));
	if(!TimeOutVar)
		outb(0x64, 0xD1);
	while((inb(0x64) & 0x02) && (TimeOutVar < 0x9C00)) {
		nop();
		TimeOutVar++;
	}
	TimeOutVar = ((TimeOutVar < 0x9C00) ? (0) : (0xFFF0));
	if(!TimeOutVar)
		outb(0x60, 0xFE);

	// IF WE MAKE IT PAST HERE THERE IS AN ERROR OR NO KEYBOARD CONTROLLER...

	#ifdef DEBUG
		DEBUG_printf("\nERROR REBOOTING USING i8042:\n");
		DEBUG_printf("%s:%d (%s)\n", __FILE__, __LINE__, __func__);
	#endif
}

int stringf(char *str, const char *format, va_list ap)
{
	size_t loc=0;
	size_t i;
	for (i=0 ; i<=strlen((const uint8_t *) format);i++, loc++)
	{
		switch (format[i]) {
			case '%':
				switch (format[i+1])
				{
					/*** characters ***/
					case 'c': {
						char c = (char) va_arg (ap, char);
						str[loc] = c;
						i++;
						break;
					}
					/*** integers ***/
					case 'd':
					case 'i':
					{
						uint32_t c = va_arg (ap, uint32_t);
						uint8_t s[32]={0};
						itoa_s (c, 10, s);
						strcpy ((uint8_t *) &str[loc], s);
						loc+= strlen(s) - 2;
						i++;		// go to next character
						break;
					}
					/*** display in hex ***/
					case 'X':
					case 'x':
					{
						uint32_t c = va_arg (ap, uint32_t);
						uint8_t s[32]={0};
						itoa_s (c,16,s);
						strcpy ((uint8_t *) &str[loc], s);
						i++;		// go to next character
						loc+=strlen(s) - 2;
						break;
					}
					/*** strings ***/
					case 's':
					{
						char *c = va_arg (ap, char*);
						char s[32]={0};
						strcpy ((uint8_t *) s, (const uint8_t*) c);						
						strcpy ((uint8_t *) &str[loc], (const uint8_t*) s);
						i++;		// go to next character
						loc+=strlen((const uint8_t*) s) - 2;
						break;
					}
					case '%':
					{
						str[loc] = '%';
						i++;
						break;
					}
				}
				break;
			default:
				str[loc] = format[i];
				break;
		}
	}
	return i;
}

#ifdef DEBUG
	int x, y = 0;
	int maxX = 80;
	int maxY = 25;
	void		DEBUG_putch				(const char charactor)
	{
		outb(DEBUG_PORT, (uint8_t) charactor);
		/*char *vidmem = (char *) 0xB8000;
		int offset = (y * maxX + x) * 2;
		switch(charactor) {
			case '\n':
				y++;
				x = 0;
				for(offset = ((y * maxX) * 2); offset <= ((y * maxX + maxX) * 2); offset++)
					vidmem[offset] = 0;
				break;
			case '\t':
				x+=5;
				break;
			default:
				vidmem[offset] = (char) charactor;
				vidmem[offset+1] = (char) 0x0F;
				x++;
		}
		if (x > maxX) {
			y++;
			x = 0;
			for(offset = ((y * maxX) * 2); offset <= ((y * maxX + maxX) * 2); offset++)
				vidmem[offset] = 0;
		}
		if (y > maxY) {
			y = 0;
			x = 0;
			for(offset = ((y * maxX) * 2); offset <= ((y * maxX + maxX) * 2); offset++)
				vidmem[offset] = 0;
		}*/
	}

	void		DEBUG_print				(const char *String)
	{
		while (*String!=0)
			DEBUG_putch((const char) *String++);
			// outb(DEBUG_PORT, (uint8_t) *String++);
	}

	void		DEBUG_printf			(const char *String, ...)
	{
		va_list ap;
		va_start(ap, String);
		char msg[2000];
		stringf(msg, String, ap);
		DEBUG_print(msg);
	}
#endif