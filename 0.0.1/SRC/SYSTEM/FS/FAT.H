/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					SYSTEM/FS/FAT.H					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FAT_H_INCLUDED
	#define FAT_H_INCLUDED

	#include <FORMATTING.H>

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

	typedef struct _FAT_DIR_SECTOR {
		FATDIR		DIRENT[16];
	} DIRSEC, *PDIRSEC;

	typedef struct _TEMP_DIR {
		DIRSEC		Sector[4096];
	} TDIR, *PTDIR;

	void _FAT_init(void);
	
#endif