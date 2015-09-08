/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			       SYSTEM/FS/VFS.c	 			 	 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "../DEVICE/DISKMAN.H"
#include "../MEM/PHYSICAL.H"
#include "VFS.H"
#include <STDIO.H>
#include <STRING.H>
#include <MATH.H>


#define DEBUG

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   Declare Local Functions				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

error _VFS_INIT_DISK_FS(uint16_t PartID);

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Variables					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define MaxDrives 1024		// We will use 0: to 3FF:

PVFSDIR CurDIR;
FILESYSTEM FSList[256];

uint16_t TotalDisks;

DISK DISKS[1024];

char *ExpPath[100];

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

error _VFS_INIT_DISK_FS(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 40) _VFS_INIT_DISK_FS(0x%x)\n\r", DiskID);
#endif
	error (*MNT)(PDISK Disk);
    MNT = FSList[DISKS[DiskID].FSysID].MOUNT;
    if (MNT)
    {
        return MNT(&DISKS[DiskID]);
    }
    return ERROR_DRIVER;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	  Public Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _VFS_DIR(const char* Path)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 58) _VFS_DIR(%s)\n\r", Path);
#endif
	memset(CurDIR, 0, sizeof(VFSDIR)); // Clear Directory List
	if(!strlen(Path) || Path[0] != '/')
		return; //error
//	printf("strlen of \"%s\" = %i\n", Path, strlen(Path));
	if(strlen(Path) == 2) {		//VFS Root
		for(uint16_t x = 0; x < TotalDisks; x++) {
			char TName[36] = { 0 };
			(void) stringf(TName, "%x", x);
			strcpy (CurDIR->File[x].name, TName);
			CurDIR->File[x].flags = FS_DISK;
			CurDIR->File[x].DiskID = x;
			CurDIR->File[x].FSysID = Get_FSysID(x);
		}
	} else {					//FS Root
		int MAX = explode(ExpPath, Path, '/');

		for (int M = 0; M < MAX; M++)
			printf("\t%i = %s\n", M, ExpPath[M]);

		uint16_t LDisk = (uint16_t) textTOhex(ExpPath[1]);
		if(LDisk < TotalDisks) {
			char *LPath = calloc(1);
			void (*FSDIR) (uint16_t DiskID, PVFSDIR DIR, const char* Path);
		    FSDIR = FSList[DISKS[LDisk].FSysID].DIR;
		    if (FSDIR) {
		    	if(MAX == 2)
		    		arrayAppend(LPath, '/');
		    	else
			    	for(int M = 2; M < MAX; M++) //Build LPath...
			    		stringf(LPath, "%s/%s", LPath, ExpPath[M]);
		        FSDIR(LDisk, CurDIR, LPath);
		    } else
		    	printf("No FileSystem Driver Found\n");
			free(LPath);
		}
		explodeFREE(ExpPath, MAX);
	}
	for(int x = 0; x < 1024; x++) {
		if(CurDIR->File[x].FSysID) {
			if(CurDIR->File[x].flags == FS_FILE)
				printfc(0xF, "%s\t", CurDIR->File[x].name);
			else if(CurDIR->File[x].flags == FS_DIRECTORY)
				printfc(0x1, "%s\t", CurDIR->File[x].name);
			else if(CurDIR->File[x].flags == FS_DISK)
				printfc(0x4, "%s\t", CurDIR->File[x].name);
			txf(1, ((CurDIR->File[x].flags == FS_DIRECTORY) ? "[%s]\t0x%x\t0x%x\n\r" : "%s\t0x%x\t0x%x\n\r"), CurDIR->File[x].name, CurDIR->File[x].currentCluster, CurDIR->File[x].fileLength);
		}
	}
	putch('\n');
}

void _VFS_init()
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 114) _VFS_init()\n\r");
#endif
	memset(&FSList, 0, sizeof(FSList));
	memset(&DISKS, 0, sizeof(DISKS));
	CurDIR = (PVFSDIR) calloc(0x400);
	TotalDisks = 0;
}

void Install_FS(FILESYSTEM FSys, uint8_t FSysID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 125) Install_FS(FILESYSTEM, 0x%x)\n\r", FSysID);
#endif
	FSList[FSysID] = FSys;
}

void Mount_Disk(uint16_t PartID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 133) Mount_Disk(0x%x)\n\r", PartID);
#endif
	DISKS[TotalDisks].PartID = PartID;
	DISKS[TotalDisks].FSysID = GetFSysID(PartID);
	if(_VFS_INIT_DISK_FS(TotalDisks))
		printf("No Driver For FSysID 0x%x\n", GetFSysID(PartID));
	else
		TotalDisks++;
}

uint64_t Get_OffsetDataStart(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 146) Get_OffsetDataStart(0x%x) = 0x%x\n\r", DiskID, DISKS[DiskID].OffsetDataStart);
#endif
	return DISKS[DiskID].OffsetDataStart;
}

uint8_t Get_SectorsPerCluster(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 154) Get_SectorsPerCluster(0x%x) = 0x%x\n\r", DiskID, DISKS[DiskID].SectorsPerCluster);
#endif
	return DISKS[DiskID].SectorsPerCluster;
}

uint32_t Get_OffsetFatStart(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 162) Get_OffsetFatStart(0x%x) = 0x%x\n\r", DiskID, DISKS[DiskID].PartID);
#endif
	return DISKS[DiskID].FATOffset;
}

uint16_t Get_PartID(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 170) Get_PartID(0x%x) = 0x%x\n\r", DiskID, DISKS[DiskID].PartID);
#endif
	return DISKS[DiskID].PartID;
}

uint8_t Get_FSysID(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 178) Get_FSysID(0x%x) = 0x%x\n\r", DiskID, DISKS[DiskID].FSysID);
#endif
	return DISKS[DiskID].FSysID;
}

uint32_t Get_ROOTOFFSET(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 186) Get_ROOTOFFSET(0x%x) = 0x%x\n\r", DiskID, DISKS[DiskID].ROOTOFFSET);
#endif
	return DISKS[DiskID].ROOTOFFSET;
}

uint16_t Get_RootCount(uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(VFS.C:Line 194) Get_RootCount(0x%x) = 0x%x\n\r", DiskID, DISKS[DiskID].RootCount);
#endif
	return DISKS[DiskID].RootCount;
}
