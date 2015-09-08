/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			  SYSTEM/DEVICE/DISKMAN.H				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef DISKMAN_H_INCLUDED
	#define DISKMAN_H_INCLUDED

	#include <FORMATTING.H>


	typedef struct PARTITONMAN
	{
		uint8_t DiskID;
		uint8_t FSysID;
		uint64_t StartLBA;
		uint64_t SectorCount;
	} PARTITION;

	void _DISKMAN_init(void);
	error Read_Disk(uint8_t Disk, void *Buffer, uint64_t Start, uint16_t count);
	error Write_Disk(uint8_t Disk, void *Buffer, uint64_t Start, uint16_t count);
	error Read_Partition(uint16_t Partition, void *Buffer, uint64_t Start, uint16_t count);
	error Write_Partition(uint16_t Partition, void *Buffer, uint64_t Start, uint16_t count);
	void Install_Disk(uint8_t LOCALID, uint64_t sectors, void (*IN)(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count), void (*OUT)(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count));
	uint8_t GetDiskCount(void);
	uint16_t GetPartitionCount(void);
	uint8_t GetFSysID(uint16_t part);

#endif