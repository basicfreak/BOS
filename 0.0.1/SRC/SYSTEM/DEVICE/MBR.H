/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			      SYSTEM/DEVICE/MBR.H	 			 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef MBR_H_INCLUDED
	#define MBR_H_INCLUDED

	#include <FORMATTING.H>

	typedef struct MBRINFOS {
		uint8_t active[32];
		uint8_t FSysID[32];
		uint64_t StartLBA[32];
		uint64_t SectorCount[32];
	} MBR;

	void getMBR(MBR *data, uint8_t DiskID);

#endif