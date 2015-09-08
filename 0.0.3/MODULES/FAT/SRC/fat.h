#ifndef				_FAT_H_INCLUDED
	#define				_FAT_H_INCLUDED
	
	#include <api.h>
	#include <vfs.h>
	#include <hal.h>

	#define FAT12FSid 0x01
	#define FAT16FSid 0x06
	#define FAT32FSid 0x0B
	#define FAT32FS2id 0x0C

	typedef struct _FAT121632_BPB {
		uint8_t		OEMName[8];
		uint16_t	BytesPerSector;
		uint8_t		SectorsPerCluster;
		uint16_t	ReservedSectors;
		uint8_t		NumberOfFats;
		uint16_t	NumDirEntries;
		uint16_t	NumSectors;
		uint8_t		Media;
		uint16_t	SectorsPerFat;
		uint16_t	SectorsPerTrack;
		uint16_t	HeadsPerCyl;
		uint32_t	HiddenSectors;
		uint32_t	LongSectors;
	}__attribute__((packed)) FATBPB, *PFATBPB;

	typedef struct _FAT1216_BPB_EXT {
		uint8_t		DriveNumber;
		uint8_t		Flags;
		uint8_t		Signiture;
		uint32_t	Serial;
		char		VolumeLable[11];
		char		SysIDString[8];
	}__attribute__((packed)) FAT1216BPBEXT, *PFAT1216BPBEXT;

	typedef struct _FAT32_BPB_EXT {
		uint32_t	SectorsPerFat32;
		uint16_t	Flags;
		uint16_t	Version;
		uint32_t	RootCluster;
		uint16_t	InfoCluster;
		uint16_t	BackupBoot;
		uint16_t	Reserved[6];
		uint8_t		DriveNumber;
		uint8_t		flag;
		uint8_t		Signiture;
		uint32_t	Serial;
		char		VolumeLable[11];
		char		SysIDString[8];
	}__attribute__((packed)) FAT32BPBEXT, *PFAT32BPBEXT;

	typedef struct _FAT1216_BS {
		uint8_t			Ignore[3];			//first 3 bytes are ignored
		FATBPB			Bpb;
		FAT1216BPBEXT	BpbExt;
		uint8_t			Filler[448];		//needed to make struct 512 bytes
		uint16_t		BootSig;
	}__attribute__((packed)) FAT1216BS, *PFAT1216BS;

	typedef struct _FAT32_BS {
		uint8_t			Ignore[3];			//first 3 bytes are ignored
		FATBPB			Bpb;
		FAT32BPBEXT		BpbExt;
		uint8_t			Filler[420];		//needed to make struct 512 bytes
		uint16_t		BootSig;
	}__attribute__((packed)) FAT32BS, *PFAT32BS;

/*Offset (in bytes) Length (in bytes) Description
0 8 Filename (but see notes below about the first byte in this field)
8 3 Extension
11 1 Attributes (see details below)
12 2 Reserved
14 2 Creation Time
16 2 Creation Date
18 2 Last Access Date
20 2 Ignore in FAT12
22 2 Last Write Time
24 2 Last Write Date
26 2 First Logical Cluster
28 4 File Size (in bytes)*/

	typedef struct _FAT_DIRECTORY {
		uint8_t		Filename[11];
		uint8_t		Attrib;
		uint8_t		Reserved;
		uint8_t		TimeCreatedMs;
		uint16_t	TimeCreated;
		uint16_t	DateCreated;
		uint16_t	DateLastAccessed;
		uint16_t	FirstClusterHiBytes;
		uint16_t	LastModTime;
		uint16_t	LastModDate;
		uint16_t	FirstCluster;
		uint32_t	FileSize;
	}__attribute__((packed)) FATDIR, *PFATDIR;

	#define FAT_ATTRIB_RO 0x01
	#define FAT_ATTRIB_HIDDEN 0x02
	#define FAT_ATTRIB_SYS 0x04
	#define FAT_ATTRIB_VOL 0x08
	#define FAT_ATTRIB_DIR 0x10
	#define FAT_ATTRIB_ARCHIVE 0x20

#endif