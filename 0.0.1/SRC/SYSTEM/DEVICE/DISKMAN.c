/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			    SYSTEM/DEVICE/DISKMAN.c	 			 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "DISKMAN.H"
#include "MBR.H"
#include "../FS/VFS.H"
#include <STDIO.H>

#define DEBUG

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   Declare Local Functions				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */



/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Variables					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint8_t TotalDISKS;
uint16_t TotalPARTITIONS;

struct DISKMAN
{
	void (*IN)(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count);
	void (*OUT)(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count);
	uint8_t LOCALID;
	uint64_t SECOTRS;
	error ERRORCODE;
} LOCALDISKS[255];

PARTITION PARTITIONS[1024];

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */



/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	  Public Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _DISKMAN_init()
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 46) _DISKMAN_init()\n\r");
#endif
	TotalDISKS = 0;
	TotalPARTITIONS = 0;
	memset(&LOCALDISKS, 0, sizeof(LOCALDISKS));
	memset(&PARTITIONS, 0, sizeof(PARTITIONS));
}

error Read_Disk(uint8_t Disk, void *Buffer, uint64_t Start, uint16_t count)
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 57) Read_Disk(0x%x, *0x%x, 0x%x, 0x%x)\n\r", Disk, Buffer, Start, count);
#endif
	if(Disk >= TotalDISKS)												// Exists?
		return ERROR_INPUT;
	if(Start+(uint64_t)count > LOCALDISKS[Disk].SECOTRS)				// In Bounds?
		return ERROR_INPUT;
	uint8_t LID = LOCALDISKS[Disk].LOCALID;
	LOCALDISKS[Disk].IN(LID, (error*) &LOCALDISKS[Disk].ERRORCODE, Buffer, Start, count);
	return LOCALDISKS[Disk].ERRORCODE;
}

error Write_Disk(uint8_t Disk, void *Buffer, uint64_t Start, uint16_t count)
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 71) Write_Disk(0x%x, *0x%x, 0x%x, 0x%x)\n\r", Disk, Buffer, Start, count);
#endif
	if(Disk >= TotalDISKS)												// Exists?
		return ERROR_INPUT;
	if(Start+(uint64_t)count > LOCALDISKS[Disk].SECOTRS)				// In Bounds?
		return ERROR_INPUT;
	uint8_t LID = LOCALDISKS[Disk].LOCALID;
	LOCALDISKS[Disk].OUT(LID, (error*) &LOCALDISKS[Disk].ERRORCODE, Buffer, Start, count);
	return LOCALDISKS[Disk].ERRORCODE;
}

error Read_Partition(uint16_t Partition, void *Buffer, uint64_t Start, uint16_t count)
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 85) Read_Partition(0x%x, *0x%x, 0x%x, 0x%x)\n\r", Partition, Buffer, Start, count);
#endif
	if(Partition >= TotalPARTITIONS)									// Exists?
		return ERROR_INPUT;
	if((Start + (uint64_t) count) > PARTITIONS[Partition].SectorCount)	// In Bounds?
		return ERROR_INPUT;
	uint8_t LID = PARTITIONS[Partition].DiskID;
	return Read_Disk(LID, Buffer, (Start+PARTITIONS[Partition].StartLBA), count);
}

error Write_Partition(uint16_t Partition, void *Buffer, uint64_t Start, uint16_t count)
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 102) Write_Partition(0x%x, *0x%x, 0x%x, 0x%x)\n\r", Partition, Buffer, Start, count);
#endif
	if(Partition >= TotalPARTITIONS)									// Exists?
		return ERROR_INPUT;
	if((Start + (uint64_t) count) > PARTITIONS[Partition].SectorCount)	// In Bounds?
		return ERROR_INPUT;
	uint8_t LID = PARTITIONS[Partition].DiskID;
	return Write_Disk(LID, Buffer, (Start+PARTITIONS[Partition].StartLBA), count);
}

void Install_Disk(uint8_t LOCALID, uint64_t sectors, void (*IN)(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count), void (*OUT)(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count))
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 115) Install_Disk(0x%x, 0x%x, void* 0x%x, void* 0x%x)\n\r", LOCALID, sectors, IN, OUT);
#endif
	LOCALDISKS[TotalDISKS].IN = IN;
	LOCALDISKS[TotalDISKS].OUT = OUT;
	LOCALDISKS[TotalDISKS].ERRORCODE = ERROR_NONE;
	LOCALDISKS[TotalDISKS].SECOTRS = sectors;
	LOCALDISKS[TotalDISKS].LOCALID = LOCALID;
	TotalDISKS++;
	if(sectors) {
		if(sectors < 0x1800) {		// If less than 3MB assume it has no MBR (LIKELY FLOPPY) (FORCE FAT12!!!!)
			// Add Partiton as whole disk
			PARTITIONS[TotalPARTITIONS].DiskID = (uint8_t)(TotalDISKS-1);
			PARTITIONS[TotalPARTITIONS].FSysID = 1;
			PARTITIONS[TotalPARTITIONS].StartLBA = 0;
			PARTITIONS[TotalPARTITIONS].SectorCount = sectors;
			TotalPARTITIONS++;
			Mount_Disk((uint16_t)(TotalPARTITIONS-1));
		} else {
			// MBR CHECKING
			MBR temp;
			memset(&temp, 0, sizeof(temp));
			getMBR(&temp, (uint8_t)(TotalDISKS-1));
			for(int x = 0; x < 32; x++)
				if (temp.FSysID[x]) {
						PARTITIONS[TotalPARTITIONS].DiskID = (uint8_t)(TotalDISKS-1);
						PARTITIONS[TotalPARTITIONS].FSysID = temp.FSysID[x];
						PARTITIONS[TotalPARTITIONS].StartLBA = temp.StartLBA[x];
						PARTITIONS[TotalPARTITIONS].SectorCount = temp.SectorCount[x];
						TotalPARTITIONS++;
						Mount_Disk((uint16_t)(TotalPARTITIONS-1));
				}
			
		}
	}
}

uint8_t GetDiskCount()
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 154) GetDiskCount() = 0x%x\n\r", TotalDISKS);
#endif
	return TotalDISKS;
}

uint16_t GetPartitionCount()
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 162) GetPartitionCount() = 0x%x\n\r", TotalPARTITIONS);
#endif
	return TotalPARTITIONS;
}

uint8_t GetFSysID(uint16_t part)
{
#ifdef DEBUG
	txf(1, "(DISKMAN.C:Line 170) GetFSysID(0x%x) = 0x%x\n\r", part, PARTITIONS[part].FSysID);
#endif
	return PARTITIONS[part].FSysID;
}
