#include <stdlib.h>
#include <memlib.h>
#include <pcilib.h>
#include <systemcalls.h>
#include <vfs.h>


typedef struct VFSQuery {
	uint32_t Flags;
	uint32_t PartID;
	uint32_t ThreadID;
	void* Data;
	char Path[1008];
} __attribute__((packed)) Query_t, *Query_p;

typedef struct DiskMan {
	uint32_t DriverID;
	uint32_t PrivateID;
} __attribute__((packed)) Disk_t, *Disk_p;

typedef struct PartMan {
	uint32_t DiskID;
	uint32_t FSID;
	uint32_t Start;
	uint32_t Length;
	char Name[48];
} __attribute__((packed)) Part_t, *Part_p;

typedef struct FileSystem {
	uint32_t DriverID;
	char Name[28];
} __attribute__((packed)) FileSystem_t, *FileSystem_p;

volatile Query_p QueryList;
volatile Disk_p DiskList;
volatile Part_p PartList;
volatile FileSystem_p FSList;

#define MAX_QUERY 256
#define MAX_DISK 512
#define MAX_PART 512
#define MAX_FS 256

volatile uint32_t Querys = 0;
volatile uint32_t Disks = 0;
volatile uint32_t Parts = 0;

void IPC_Thread(void);
void QueryThread(void);

int init()
{
	initHeap();
	QueryList = (Query_p) calloc((sizeof(Query_t) * MAX_QUERY));
	DiskList = (Disk_p) calloc((sizeof(Disk_t) * MAX_DISK));
	PartList = (Part_p) calloc((sizeof(Part_t) * MAX_PART));
	FSList = (FileSystem_p) calloc((sizeof(FileSystem_t) * MAX_FS));
	_TM_SETNAME("VFS-Main");
	_TM_FORK_NOCOW((uint32_t)&IPC_Thread, "VFS");
	QueryThread();
}

void IPC_Thread()
{
	uint8_t* MessageData = (uint8_t*) 0xD0000000;
	while(1) {
		_IPC_WAITMSG(0); // wait for any message
		switch (MessageData[0]) {
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
		}
	}
}

void QueryThread()
{
	while (1) {
		if(!Querys) {
			_yeild();
		} else {
			//
		}
	}
}