/*
./LIBSRC/STRING.C
*/

#include <STRING.H>
#include <STDIO.H>
#include "../SYSTEM/MEM/PHYSICAL.H"

bool arrayAppend(char* out, const char in)
{
	bool ret = TRUE;
	int i = strlen(out) - 1;
	if (!(out[i] = in) )
		ret = FALSE;
	if ((out[i+1] = '\0') )
		ret = FALSE;
	return ret;
}

bool arrayRemove(char* out, const int cs)
{
	bool ret = TRUE;
	int i = strlen(out) - 1;
	if ( i <= 0 ) return FALSE;
	for (i = strlen(out) - 1 - cs; i < strlen(out) - 1; i++)
		if ((out[i] = '\0'))
			ret = FALSE;
	return ret;
}

int searchChar(char find, char *in)
{
	int ret;
	int done;
	for (ret = 0; done == 0; ret++)
	{
		if (in[ret] == find) done = 1;
		if (ret == strlen(in))
		{
			done = 1;
			ret = 0;
		}
	}
	return ret;
}

int strlen(const char *str)
{
	size_t ret = 0;
	while (str[ret++]);
	return ret;
}

int explode( char *out[], const char *s1, const char s2 )
{
    int i = 0;
	uint32_t z = 0;
	out[0] = malloc(1);
	while (*s1 != 0)
	{
		if(*s1 != s2)
		{
			out[i][z] = *s1++;
			z++;
//			putch('-');
		}
		else
		{
//			putch(';');
			out[i][z] = (char) 0;
//			printf("\"%s\"\n", out[i]);
			i++;
			out[i] = malloc(1);
//			putch('.');
			z = (uint32_t) (*s1++ & 0);
		}
	}
//	putch('?');
	out[i][z] = (char) 0;
//	printf("\"%s\"\n", out[i]);
    i++;
    return i;
}

void explodeFREE( char *out[], int len)
{
	for(int i = 0; i < len; i++)
		free(out[i]);
}

bool streql( const char *s1, const char *s2 )
{
	int temp = strcmp (s1, s2);
	if (temp == 0) return TRUE;
	else return FALSE;
}

int strcmp( const char * s1, const char * s2 )
{
	int ret = 0;
	while (!(ret = *(unsigned char*)s1 - *(unsigned char*)s2) && *s2)
    {
        ++s1;
        ++s2;
    }
	if (ret < 0) ret = -1;
	if (ret > 0) ret = 1;
    return	ret;
}

char *strcpy(char *s1, const char *s2)
{
	char *s1_p = s1;
	while ( (*s1++ = *s2++) );
	return s1_p;
}

char* StringtoUpper(char* in)
{
	char* out = (char *) "                                                  ";
	int i = 0;
	while (*in != 0) {
		out[i] = ChartoUpper(*in++);
		i++;
	}
	out[i] = '\0';
	return out;
}

char* StringtoLower(char* in)
{
	char* out = (char *) "                                                  ";
	int i = 0;
	while (*in != 0) {
		//out[i] = ChartoLower(*in);
		if(*in >= 'A' && *in <= 'Z')
			out[i] = (char) (*in++ +  32);
		else
			out[i] = *in++;
		i++;
	}
	out[i] = '\0';
	return out;
}

char ChartoUpper(char in)
{
	char out;
	if(in >= 'a' && in <= 'z')
		out = (char) (in - 32);
	else
		out = in;
	return out;
}

char ChartoLower(char in)
{
	char out;
	if(in >= 'A' && in <= 'Z')
		out = (char) (in + 32);
	else
		out = in;
	return out;
}


char* strchr (char * str, int character )
{
	do {
		if ( *str == character )
			return (char*)str;
	}
	while (*str++);
	return 0;
}

char* charchange(const char in)
{
	char *ret = (char*) "     ";
	ret[0] = in;
	ret[1] = '\0';
	return ret;
}

int sf(char *str, const char *format, va_list ap)
{
	size_t loc=0;
	size_t i;
	for (i=0 ; i<=strlen(format);i++, loc++)
	{
		switch (format[i]) {
			case '%':
				switch (format[i+1])
				{
					/*** characters ***/
					case 'c': {
						char c = va_arg (ap, char);
						str[loc] = c;
						i++;
						break;
					}
					/*** integers ***/
					case 'd':
					case 'i':
					{
						unsigned int c = va_arg (ap, unsigned int);
						char s[32]={0};
						itoa_s (c, 10, s);
						strcpy (&str[loc], s);
						loc+= strlen(s) - 2;
						i++;		// go to next character
						break;
					}
					/*** display in hex ***/
					case 'X':
					case 'x':
					{
						uint32_t c = va_arg (ap, uint32_t);
						char s[32]={0};
						itoa_s (c,16,s);
						strcpy (&str[loc], s);
						i++;		// go to next character
						loc+=strlen(s) - 2;
						break;
					}
					/*** strings ***/
					case 's':
					{
						char *c = va_arg (ap, char*);
						char s[32]={0};
						strcpy (s,(const char*)c);						
						strcpy (&str[loc], s);
						i++;		// go to next character
						loc+=strlen(s) - 2;
						break;
					}
					case '%':
					{
						putch('%');
						i++;
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
int stringf(char *str, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	return sf(str, format, ap);
}



char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(unsigned i,unsigned base,char* buf)
{
   int pos = 0;
   int opos = 0;
   int top = 0;

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

void itoa_s(unsigned i,unsigned base,char* buf)
{
   if (base > 16) return;
   itoa(i,base,buf);
   return;
}