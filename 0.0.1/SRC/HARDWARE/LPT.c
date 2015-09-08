/*
./DRIVERSRC/LPT.C
*/

#include "LPT.H"
#include "BIOS.H"
#include <STDIO.H>

void LPT1_OUT(uint8_t data)
{
	outb (_BIOS_LPT(1), data);
}

uint8_t LPT1_IN()
{
	return inb (_BIOS_LPT(1));
}

void LPT2_OUT(uint8_t data)
{
	outb (_BIOS_LPT(2), data);
}

uint8_t LPT2_IN()
{
	return inb (_BIOS_LPT(2));
}

void LPT3_OUT(uint8_t data)
{
	outb (_BIOS_LPT(3), data);
}

uint8_t LPT3_IN()
{
	return inb (_BIOS_LPT(3));
}
