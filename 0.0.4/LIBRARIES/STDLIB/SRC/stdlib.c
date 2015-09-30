#include <stdlib.h>
#include <stdarg.h>

void _itoa(uint32_t, uint32_t, uint8_t*);
void _itoa_s(uint32_t, uint32_t, uint8_t*);

void Bochs_putch(const char chr)
{
	__asm__ __volatile__("outb %0, %1" : : "a"(chr), "Nd"(0xE9));
}

void Bochs_puts(const char* Str)
{
	while(*Str) {
		__asm__ __volatile__("outb %0, %1" : : "a"(*Str++), "Nd"(0xE9));
	}
}

void Bochs_printf(const char* Str, ...)
{
	va_list ap;
	va_start(ap, Str);
	while(*Str) {
		if(*Str == '%') {
			*Str++;
			switch(*Str) {
				/*** characters ***/
				case 'c': {
					const char c = (const char) va_arg (ap, const char);
					Bochs_putch(c);
					*Str++;
					break;
				}
				/*** integers ***/
				case 'd':
				case 'i':
				{
					uint32_t c = va_arg (ap, uint32_t);
					uint8_t s[32]={0};
					_itoa_s (c, 10, s);
					Bochs_puts((const char*) s);
					*Str++;		// go to next character
					break;
				}
				/*** display in hex ***/
				case 'X':
				case 'x':
				{
					uint32_t c = va_arg (ap, uint32_t);
					uint8_t s[32]={0};
					_itoa_s (c,16,s);
					Bochs_puts((const char*) s);
					*Str++;		// go to next character
					break;
				}
				/*** strings ***/
				case 's':
				{
					const char *s = va_arg (ap, const char*);
					Bochs_puts((const char*) s);
					*Str++;		// go to next character
					break;
				}
				case '%':
				{
					Bochs_putch('%');
					*Str++;
					break;
				}
			}
		} else {
			Bochs_putch(*Str++);
		}
	}
}

const uint8_t		bchars[] =				{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void _itoa(uint32_t i, uint32_t base, uint8_t* buf)
{
   uint32_t pos = 0;
   uint32_t opos = 0;
   uint32_t top = 0;
   uint8_t tbuf[32] = { 0 };
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

void _itoa_s(uint32_t i, uint32_t base, uint8_t* buf)
{
   if (base > 16) return;
   _itoa(i,base,buf);
   return;
}

void memset(void* Destination, uint8_t Data, uint32_t Length)
{
	while(Length)
		((uint8_t*)Destination)[Length--] = Data;
}

void memsetw(void* Destination, uint16_t Data, uint32_t Length)
{
	while(Length)
		((uint16_t*)Destination)[Length--] = Data;
}

void memsetd(void* Destination, uint32_t Data, uint32_t Length)
{
	while(Length)
		((uint32_t*)Destination)[Length--] = Data;
}
