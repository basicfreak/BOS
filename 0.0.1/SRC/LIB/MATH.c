/*
./LIBSRC/MATH.C
*/

#include <MATH.H>
#include <STRING.H>

int exponent(int base, unsigned int exp)
{
    unsigned int i;
    int result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
}

unsigned int textTOhex(char* s)
{
	unsigned int ret = 0;
	while ( *s != 0 )
	{
		if (*s >= '0' && *s <= '9') ret = (unsigned int) ((unsigned int) (ret * 16) + (unsigned int) (*s++ - '0'));
		else if (*s >= 'A' && *s <= 'F') ret = (unsigned int) ((unsigned int) (ret * 16) + (unsigned int) (*s++ - 'A' + 10));
		else if (*s >= 'a' && *s <= 'f') ret = (unsigned int) ((unsigned int) (ret * 16) + (unsigned int) (*s++ - 'a' + 10));
		else return (unsigned int) (*s++ & 0);
	}
	return ret;
}

char* hexTOtext(unsigned int in)
{
	char* out = (char *) "        ";
	itoa_s (in, 16, out);
	return out;
}

char* intTOchar(unsigned int in)
{
	char* out = (char *) "        ";
	itoa_s (in, 10, out);
	return out;
}

uint64_t charTOint(const char* in)
{
	uint64_t ret = 0;
	while (*in != 0)
	{
		if (*in < '0' || *in > '9') return 0;
		ret = ret * 10 + (uint64_t)(*in++ - '0');
	}
	return ret;
}

uint64_t BIN2INT(unsigned char* in)
{
	int i = strlen((const char*)in) -1;
	uint64_t ret = 0;
	while (i >= 0) {
		ret = ret * 256 + in[i];
		i--;
	}
	//ret = (uint64_t) in;
	return ret;
}

bool isEven(uint64_t in)
{
	if (in%2 == 0) return true;
	return false;
}
