/*
./INCLUDE/MATH.H
*/

#include <FORMATTING.H>

#ifndef				MATH_H_INCLUDED
	#define				MATH_H_INCLUDED
	
	int exponent(int base, unsigned int exp);
	unsigned int textTOhex(char* s);
	char* hexTOtext(unsigned int in);
	char* intTOchar(unsigned int in);
	uint64_t charTOint(const char* in);
	uint64_t BIN2INT(unsigned char* in);
	bool isEven(uint64_t in);
	
#endif