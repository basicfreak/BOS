#ifndef __FAT_HEADER_INCLUDED__
	#define __FAT_HEADER_INCLUDED__

	#include <stdint.h>

	extern int ReadSectors(void*, int, int);
	extern int WriteSectors(void*, int, int);

	int FAT_Open(void*); // Path
	int FAT_Read(void*); // Buffer, Offset, Size
	int FAT_Write(void*, int, int); // Buffer, Offset, Size
	int FAT_Close(void);
	static void FAT_INIT(void);
	static int ReadSectors_Safe(void*, int, int);
	static uint32_t NextCluster(uint32_t);
	static int ReadCluster(void*, uint32_t);
	static int ConvertPath(char*, char*);
	static uint32_t FindFile(char*, int);
	static uint32_t SearchDir(void*, char*, int);

	typedef struct BPB_STRUCT {
		uint8_t 	JumpCode[3];
		char		OEMID[8];
		uint16_t	BytesPerSector;	/* bytes per sector */
		uint8_t		SectorsPerCluster;	/* sectors per cluster */
		uint16_t	ReservedSectors;	/* number of reserved sectors */
		uint8_t		FATs;	/* number of FATs */
		uint16_t	RootDirEnts;	/* number of root directory entries */
		uint16_t	Sectors;	/* total number of sectors */
		uint8_t		Media;	/* media descriptor */
		uint16_t	FATsectors;	/* number of sectors per FAT */
		uint16_t	SectorsPerTrack;	/* sectors per track */
		uint16_t	Heads;	/* number of heads */
		uint32_t	HiddenSectors;	/* # of hidden sectors */
		uint32_t	TotalSectors;	/* # of sectors if bpbSectors == 0 */
		uint32_t	BigFATsectors;	/* like bpbFATsecs for FAT32 */
		uint16_t	unused[2];
		uint32_t	RootCluster;	/* start cluster for root directory */
	} __attribute__((packed)) BPB_t, *BPB_p;

	typedef struct FAT_DIR {
		char		FileName[11];
		uint8_t		attribute;
		uint8_t		reserved;
		uint8_t		CreationDecaSecond;
		uint16_t	CreationTime;
		uint16_t	CreationDate;
		uint16_t	AccessDate;
		uint16_t	Cluster_High;
		uint16_t	ModTime;
		uint16_t	ModDate;
		uint16_t	Cluster_Low;
		uint32_t	FileSize;
	} __attribute__((packed)) DIR_t, *DIR_p;

	typedef struct FAT_LFN {
		uint8_t		EntryID;
		uint16_t	Unicode0[5];
		uint8_t		attribute;
		uint8_t		type;
		uint16_t	Unicode1[6];
		uint16_t	reserved;
		uint16_t	Unicode2[2];
	} __attribute__((packed)) LFN_t, *LFN_p;

#endif
