/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					SYSTEM/FS/VFS.H					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef VFS_H_INCLUDED
	#define VFS_H_INCLUDED

	#define FS_FILE			0
	#define FS_DIRECTORY	1
	#define FS_INVALID		2
 	#define FS_DISK			3

	#include <FORMATTING.H>

	typedef struct _FILE {
		char        name[35];
		uint32_t    flags;
		uint32_t    fileLength;
		uint32_t    id;
		uint32_t    eof;
		uint32_t    position;
		uint32_t    currentCluster;
		uint16_t    DiskID;
		uint8_t		FSysID;
	} FILE, *PFILE;

	typedef struct _VFSDIR {
		FILE File[65536];
	} VFSDIR, *PVFSDIR;

	typedef struct Disk_Properties {
		uint8_t SectorsPerCluster;
		uint64_t OffsetDataStart;
		uint32_t ROOTOFFSET;
		uint16_t RootCount;	//0 = cluster based / else = number of entries
		uint32_t FATOffset;
		uint16_t PartID;
		uint8_t FSysID;
	} DISK, *PDISK;

	typedef struct _FILE_SYSTEM {
		char 		Name[8];
		void		(*DIR)		(uint16_t DiskID, PVFSDIR DIR, const char* Path);
		FILE		(*Open)		(uint16_t DiskID, const char* Path);
		void		(*Close)	(PFILE File);
		void		(*Read)		(PFILE File, void *Buffer, uint32_t Length);
		void		(*Write)	(PFILE File, void *Buffer, uint32_t Length);
		error		(*MOUNT)	(PDISK Disk);
	} FILESYSTEM, *PFILESYSTEM;


	void _VFS_DIR(const char* Path);
	void _VFS_init(void);
	void Install_FS(FILESYSTEM FSys, uint8_t FSysID);
	void Mount_Disk(uint16_t PartID);
	uint64_t Get_OffsetDataStart(uint16_t DiskID);
	uint8_t Get_SectorsPerCluster(uint16_t DiskID);
	uint32_t Get_OffsetFatStart(uint16_t DiskID);
	uint16_t Get_PartID(uint16_t DiskID);
	uint8_t Get_FSysID(uint16_t DiskID);
	uint32_t Get_ROOTOFFSET(uint16_t DiskID);
	uint16_t Get_RootCount(uint16_t DiskID);

#endif