//BPB.H

#ifndef				BPB_H_INCLUDED
	#define				BPB_H_INCLUDED
/*
BPB (BOOTSECTOR):
0x03 - 0x0A		DESCRIPTION
0x0B - 0x0C		BYTES PER SECTOR
0x0D			SECTORS PER CLUSTER
0x0E - 0x0F		RESERVED SECTORS (1)
0x10			NUMBER OF FATS
0x11 - 0x12		ROOT ENTRIES
0x13 - 0x14		TOTAL SECTORS
				IF > 65535 SET TO 0
				AND STORE AT 0x20
0x15			MEDIA DESCRIPTOR
0x16 - 0x17		SECTORS PER FAT
0x18 - 0x19		SECTORS PER TRACK (HEAD)
0x1A - 0x1B		NUMBER OF HEADS
0x1C - 0x1F		HIDDEN SECTORS (0)
0x20 - 0x23		TOTAL SECTORS
0x24 - 0x25		DRIVE NUMBER
0x26			EXT BOOT RECORD SIGNATURE
0x27 - 0x2A		VOLUME SERIAL NUMBER
0x2B - 0x35		VOLUME LABEL
0x36 - 0x3D		FS LABEL
*/

	typedef struct _BIOS_PARAMATER_BLOCK {
		uint8_t			OEMName[8];
		uint16_t		BytesPerSector;
		uint8_t			SectorsPerCluster;
		uint16_t		ReservedSectors;
		uint8_t			NumberOfFats;
		uint16_t		NumDirEntries;
		uint16_t		NumSectors;
		uint8_t			Media;
		uint16_t		SectorsPerFat;
		uint16_t		SectorsPerTrack;
		uint16_t		HeadsPerCyl;
		uint32_t		HiddenSectors;
		uint32_t		LongSectors;

	}__attribute__((packed)) BIOSPARAMATERBLOCK, *PBIOSPARAMATERBLOCK;

	typedef struct _BIOS_PARAMATER_BLOCK_EXT {
		uint32_t			SectorsPerFat32;
		uint16_t			Flags;
		uint16_t			Version;
		uint32_t			RootCluster;
		uint16_t			InfoCluster;
		uint16_t			BackupBoot;
		uint16_t			Reserved[6];

	}__attribute__((packed)) BIOSPARAMATERBLOCKEXT, *PBIOSPARAMATERBLOCKEXT;

	typedef struct _BOOT_SECTOR {
		uint8_t					Ignore[3];			//first 3 bytes are ignored
		BIOSPARAMATERBLOCK		Bpb;
		BIOSPARAMATERBLOCKEXT	BpbExt;
		uint8_t					Filler[448];		//needed to make struct 512 bytes

	}__attribute__((packed)) BOOTSECTOR, *PBOOTSECTOR;



#endif
