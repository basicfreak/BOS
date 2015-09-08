/*
./LIBSRC/CMOS.C
*/

#include "CMOS.H"
#include <STDIO.H>

const char *CMOS_Floppy_Decoded[6] = {
			"None", "360KB 5.25\"", "1.2MB 5.25\"", "720KB 3.5\"", "1.44MB 3.5\"", "2.88MB 3.5\""
		};
void readCMOS()
{
	unsigned char temp;
	outb(CMOS, 0x10);
	temp = inb(CMOSdata);
	CMOS_Floppy_Master = temp >> 4;
	CMOS_Floppy_Slave = temp & 0xF;
	return;
}

const char *CMOS_Floppy_Decode(int type)
{
	if (type >= 0 && type <= 5)
		return CMOS_Floppy_Decoded[type];
	else return "ERROR";
}
