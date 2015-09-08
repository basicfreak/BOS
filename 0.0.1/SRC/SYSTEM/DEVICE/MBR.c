/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			      SYSTEM/DEVICE/MBR.c	 			 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "MBR.H"
#include "DISKMAN.H"

 #include <STDIO.H>

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   Declare Local Functions				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

int TraceExtPT(MBR *data, uint8_t DiskID, uint64_t Offset);

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Variables					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define MBRoffset 0x1BE

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

int TraceExtPT(MBR *data, uint8_t DiskID, uint64_t Offset)
{
	int ret = 0;
	uint64_t curOff = Offset;
	bool trace = true;
	while(trace) {
		trace = false;
		uint8_t myBuff[512] = { 0 };
		if(!Read_Disk(DiskID, &myBuff, curOff, 1)) {
			for(int x = 0; x < 2; x ++) {
				data->active[ret] = myBuff[MBRoffset + (x*16)];
		 		data->FSysID[ret] = myBuff[(MBRoffset + (x*16)) + 4];
		 		if(data->FSysID[ret]) {
			 		if(data->FSysID[ret] != 5 && data->FSysID[ret] != 0xF) {
				 		if(data->active[ret] == 0 || data->active[ret] == 0x80) {	//Standard MBR
				 			data->StartLBA[ret] = Offset + (uint64_t) ((myBuff[(MBRoffset + (x*16)) + 11] << 24) | (myBuff[(MBRoffset + (x*16)) + 10] << 16) | (myBuff[(MBRoffset + (x*16)) + 9] << 8) | myBuff[(MBRoffset + (x*16)) + 8]);
				 			data->SectorCount[ret] = (uint64_t) ((myBuff[(MBRoffset + (x*16)) + 15] << 24) | (myBuff[(MBRoffset + (x*16)) + 14] << 16) | (myBuff[(MBRoffset + (x*16)) + 13] << 8) | myBuff[(MBRoffset + (x*16)) + 12]);
				 			ret++;
				 		} else if (data->active[ret] == 1 || data->active[ret] == 0x81) { //LBA48 MBR
				 			data->StartLBA[ret] = Offset + (uint64_t) ((uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 2] << 40) | (uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 3] << 32) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 11] << 24) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 10] << 16) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 9] << 8) | (uint64_t)myBuff[(MBRoffset + (x*16)) + 8]);
				 			data->SectorCount[ret] = (uint64_t) ((uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 6] << 40) | (uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 7] << 32) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 15] << 24) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 14] << 16) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 13] << 8) | (uint64_t)myBuff[(MBRoffset + (x*16)) + 12]);
				 			ret++;
				 		} else { // Should not exist I hope...
				 			data->active[ret] = 0;
					 		data->FSysID[ret] = 0;
				 		}
				 	} else {		//Extended Table
				 		if(data->FSysID[ret] == 5) { //standard
				 			curOff = Offset + (uint64_t) ((myBuff[(MBRoffset + (x*16)) + 11] << 24) | (myBuff[(MBRoffset + (x*16)) + 10] << 16) | (myBuff[(MBRoffset + (x*16)) + 9] << 8) | myBuff[(MBRoffset + (x*16)) + 8]);
				 		} else {	//lba48
				 			curOff = Offset + (uint64_t) ((uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 2] << 40) | (uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 3] << 32) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 11] << 24) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 10] << 16) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 9] << 8) | (uint64_t)myBuff[(MBRoffset + (x*16)) + 8]);
				 		}
				 		trace = true;
				 	}
				}
			}
		}
	}
	return ret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	  Public Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

 void getMBR(MBR *data, uint8_t DiskID)
 {
 	uint8_t myBuff[512] = { 0 };
 	if(!Read_Disk(DiskID, &myBuff, 0, 1)) {
	 	int partid = 0;
	 	for(int x = 0; x < 4; x++)
	 	{
	 		data->active[partid] = myBuff[MBRoffset + (x*16)];
	 		data->FSysID[partid] = myBuff[(MBRoffset + (x*16)) + 4];
	 		if(data->FSysID[partid] != 5 && data->FSysID[partid] != 0xF) {
		 		if(data->active[partid] == 0 || data->active[partid] == 0x80) {	//Standard MBR
		 			data->StartLBA[partid] = (uint64_t) ((myBuff[(MBRoffset + (x*16)) + 11] << 24) | (myBuff[(MBRoffset + (x*16)) + 10] << 16) | (myBuff[(MBRoffset + (x*16)) + 9] << 8) | myBuff[(MBRoffset + (x*16)) + 8]);
		 			data->SectorCount[partid] = (uint64_t) ((myBuff[(MBRoffset + (x*16)) + 15] << 24) | (myBuff[(MBRoffset + (x*16)) + 14] << 16) | (myBuff[(MBRoffset + (x*16)) + 13] << 8) | myBuff[(MBRoffset + (x*16)) + 12]);
		 			partid++;
		 		} else if (data->active[partid] == 1 || data->active[partid] == 0x81) { //LBA48 MBR
		 			data->StartLBA[partid] = (uint64_t) ((uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 2] << 40) | (uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 3] << 32) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 11] << 24) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 10] << 16) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 9] << 8) | (uint64_t)myBuff[(MBRoffset + (x*16)) + 8]);
		 			data->SectorCount[partid] = (uint64_t) ((uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 6] << 40) | (uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 7] << 32) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 15] << 24) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 14] << 16) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 13] << 8) | (uint64_t)myBuff[(MBRoffset + (x*16)) + 12]);
		 			partid++;
		 		} else { // Should not exist I hope...
		 			data->active[partid] = 0;
			 		data->FSysID[partid] = 0;
		 		}
		 	} else {		//Extended Table
		 		uint64_t ExtStart = 0;
		 		if(data->FSysID[partid] == 5) { //standard
		 			ExtStart = (uint64_t) ((myBuff[(MBRoffset + (x*16)) + 11] << 24) | (myBuff[(MBRoffset + (x*16)) + 10] << 16) | (myBuff[(MBRoffset + (x*16)) + 9] << 8) | myBuff[(MBRoffset + (x*16)) + 8]);
		 		} else {	//lba48
		 			ExtStart = (uint64_t) ((uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 2] << 40) | (uint64_t)((uint64_t)myBuff[(MBRoffset + (x*16)) + 3] << 32) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 11] << 24) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 10] << 16) | (uint64_t)(myBuff[(MBRoffset + (x*16)) + 9] << 8) | (uint64_t)myBuff[(MBRoffset + (x*16)) + 8]);
		 		}
		 		MBR TempExtMBR;
		 		int ExtPartCount = TraceExtPT(&TempExtMBR, DiskID, ExtStart);
		 		for(int y = 0; y < ExtPartCount; y++) {
		 			if(TempExtMBR.FSysID[y]) {
			 			data->active[partid] = TempExtMBR.active[y];
			 			data->FSysID[partid] = TempExtMBR.FSysID[y];
			 			data->StartLBA[partid] = TempExtMBR.StartLBA[y];
			 			data->SectorCount[partid] = TempExtMBR.SectorCount[y];
			 			partid++;
			 		}
		 		}
		 	}
	 	}
	 }
 }
