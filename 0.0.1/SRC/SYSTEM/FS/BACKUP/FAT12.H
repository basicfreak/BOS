/*
./INCLUDE/FS/FAT12.H
*/

#ifndef				FAT_H_INCLUDED
	#define				FAT_H_INCLUDED

	#include <FORMATTING.H>
	#include "VFS.H"
	
	typedef struct _DIRECTORY {
		uint8_t   Filename[8];
		uint8_t   Ext[3];
		uint8_t   Attrib;
		uint8_t   Reserved;
		uint8_t   TimeCreatedMs;
		uint16_t  TimeCreated;
		uint16_t  DateCreated;
		uint16_t  DateLastAccessed;
		uint16_t  FirstClusterHiBytes;
		uint16_t  LastModTime;
		uint16_t  LastModDate;
		uint16_t  FirstCluster;
		uint32_t  FileSize;

	}__attribute__((packed)) DIRECTORY, *PDIRECTORY;

	typedef struct _MOUNT_INFO {
		uint32_t numSectors;
		uint32_t fatOffset;
		uint32_t numRootEntries;
		uint32_t rootOffset;
		uint32_t rootSize;
		uint32_t fatSize;
		uint32_t fatEntrySize;

	}MOUNT_INFO, *PMOUNT_INFO;
	
	typedef struct _ROOTDIRECTORY {
		PDIRECTORY entry[224];
	}RDIR, *PRDIR ;
	typedef struct _ROOTDIRECTORYS {
		DIRECTORY entry[224];
	}RDIRS, *PRDIRS ;
void dirTest(void);
	FILE fsysFatDirectory (const char* DirectoryName);
	void fsysFatRead(PFILE file, unsigned char* Buffer, unsigned int Length);
	FILE fsysFatOpen (const char* FileName);
	void fsysFatInitialize (void);
	void fsysFatMount (void);
/*
ROOT DIRECTORY:
0x00 - 0x07		FILENAME
0x08 - 0x0A		EXTENTION
0x0B			ATTRIBUTE
0x0C - 0x15		RESERVED
0x16 - 0x17		TIME CREATE OR LAST UPDATE
0x18 - 0x19		DATE CREATE OR LAST UPDATE
0x1A - 0x1B		START CLUSTER
0x1C - 0x1F		SIZE (BYTES)

FILENAME (FIRST BYTE):
0x00			FILENAME NEVER USED
0xE5			FILE DELETED
0x05			FIRST CHARACTOR IS 0xE5 (å)
0x2E			DIRECTORY
	IF SECOND IS 2E CLUSTER FIELD CONTAINS
		CLUSTER NUMBER OF PARRENT

ATTRIBUTE:
0x01			READ ONLY
0x02			HIDDEN
0x04			SYSTEM
0x08			(SPECIAL) DISK VOLUME LABLE
0x10			SUBDIRECTORY
0x20			ARCHIVE FLAG
0x40			NOT USED MUST BE 0
0x80			NOT USED MUST BE 0

TIME:
0x16:
7 6 5 4 3 2 1 0
M M M X X X X X
0x17:
7 6 5 4 3 2 1 0
H H H H H M M M
-
HHHHH			HOURS (0-23)
MMMMMM			MINUTES (0-59)
XXXXX			2SECONDS (0-29) = (0-58)

DATE:
0x18
7 6 5 4 3 2 1 0
M M M D D D D D
0x19
7 6 5 4 3 2 1 0
Y Y Y Y Y Y Y M
-
YYYYYYY			YEAR 1980 - 2099 (0-119)
MMMM			MONTH (1-12)
DDDDD			DAY (1-31)
*/




#endif