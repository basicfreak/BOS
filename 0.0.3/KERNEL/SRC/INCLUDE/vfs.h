#ifndef				VFS_H_INCLUDED
	#define				VFS_H_INCLUDED

	#include <KLIB.h>

	typedef struct Files {
		uint32_t PartID;
		uint32_t Start;
		uint32_t Length;
		uint32_t Offset;
		uint32_t Flags;
		uint32_t BOSTimeStamp; //Y/M/D/H/M/S 10/3/4/5/5/5 0-2047(1000-3047)/1-12/1-31/0-60/0-60/0-60
		char Name[32];
		// The Following are only used if Flags | FF_DIRLIST (0 = End of Linked List)
		void *PreviousEntry;
		void *NextEntry;
	}__attribute__((packed)) FILE; //64Bytes

	typedef struct FileSystems {
		void (*mount)(uint32_t);
		void (*umount)(uint32_t);
		void (*directory)(FILE *dir);
		void (*newFile)(FILE *file, char** path);
		void (*open)(FILE *file, char** path);
		void (*read)(FILE *file, void* Address, void* Buffer, uint32_t Length);
		void (*write)(FILE *file, void* Address, void* Buffer, uint32_t Length);
		char name[40];
	}__attribute__((packed)) FS_t, *FS_p; //64Bytes

	typedef struct FSDataStruct {
		void *Buffer[1024];
	}__attribute__((packed)) FSDATA_t, *FSDATA_p;

	typedef struct Partitions {
		uint16_t HALid;
		uint16_t FSid;
		uint32_t Start;
		uint32_t Length;
		uint32_t PrivateID;
		FSDATA_p FSdata;
		char Name[12];
	}__attribute__((packed)) PART_t, *PART_p; //32Bytes

	enum FileFlags {
		FF_DIRLIST = 0x8000,
		FF_DIR = 0x0001,
		FF_RO = 0x0002,
		FF_HIDDEN  = 0x0004,
		FF_SYSTEM = 0x0008,
		FF_INVALID = 0xFFFF
	};

	void _VFS_init(void);
	FILE *fdir(char* path);
	FILE *fnew(char* path);
	void fnewDIR(char* path);
	FILE *fopen(char* path);
	void fclose(FILE *file);
	void fread(FILE *file, void* Address, void* Buffer, uint32_t Length);
	void fwrite(FILE *file, void* Address, void* Buffer, uint32_t Length);
	void fmount(uint32_t);
	void fumount(uint32_t);

	uint32_t mkPartition(PART_p PartPointer);
	void rmPartition(uint32_t PartID);
	PART_p getPartition(uint32_t PartID);
	void mkFileSystem(uint16_t FSid, FS_p FSPointer);
	void rmFileSystem(uint32_t FSid);
	void PartRead(uint32_t PartID, void* Address, void* Buffer, uint32_t Length);
	void PartWrite(uint32_t PartID, void* Address, void* Buffer, uint32_t Length);

#endif