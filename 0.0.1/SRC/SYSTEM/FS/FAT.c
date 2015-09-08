/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			       SYSTEM/FS/FAT.c	 			 	 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "../DEVICE/DISKMAN.H"
#include "../MEM/PHYSICAL.H"
#include "FAT.H"
#include "VFS.H"
#include <STDIO.H>
#include <STRING.H>

#define DEBUG

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   Declare Local Functions				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _FAT_Cluster_DIR(FILE Dir, void *Buffer);
FILE _FAT_SearchDIR(PTDIR Dir, const char *Find, uint16_t DiskID);
uint32_t _FAT_GetNextCluster(PFILE File);
void _FAT_Directory(PTDIR Dir, uint16_t DiskID, const char* Path);
void _FAT_DIR(uint16_t DiskID, PVFSDIR DIR, const char* Path);
FILE _FAT_Open(uint16_t DiskID, const char* Path);
void _FAT_Close(PFILE File);
void _FAT_Read(PFILE File, void *Buffer, uint32_t Length);
void _FAT_Write(PFILE File, void *Buffer, uint32_t Length);
error _FAT_MOUNT(PDISK Disk);

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Variables					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint8_t ClusterBuffer[1024] = { 0 };

PTDIR TempDIR;

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _FAT_Cluster_DIR(FILE Dir, void *Buffer)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line 41) _FAT_Cluster_DIR(FILE, *0x%x)\n\r", Buffer);
#endif
	uint32_t NextCluster = Dir.currentCluster;
	uint8_t SecPerClust = Get_SectorsPerCluster(Dir.DiskID);
	uint64_t Offset = Get_OffsetDataStart(Dir.DiskID);
	uint16_t PartID = Get_PartID(Dir.DiskID);
	uint8_t *BUFFER = (uint8_t *) Buffer;
	int x = 0;
	uint32_t clusterTerminator = 0;
	switch (Dir.FSysID) {
		case 1:
			clusterTerminator = 0xFF0;
			break;
		case 4:
			clusterTerminator = 0xFFF0;
			break;
		case 0xB:
		case 0xC:
			clusterTerminator = 0xFFFFFF0;
			break;
		default:
			clusterTerminator = 0xFF0;
	}
	while(NextCluster < clusterTerminator) {
		if(!Read_Partition(PartID, &BUFFER[(x*512*SecPerClust)], ((NextCluster - 2) * SecPerClust)+Offset, SecPerClust)) {
			NextCluster = _FAT_GetNextCluster(&Dir);
			Dir.currentCluster = NextCluster;
			x++;
		}
	}
	printf("END OF FILE\n");
	txf(1, "END OF FILE\n\r");
}

FILE _FAT_SearchDIR(PTDIR Dir, const char *Find, uint16_t DiskID)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line 78) _FAT_SearchDIR(PTDIR, %s, 0x%x)\n\r", Find, DiskID);
#endif
	FILE tempFile;
	tempFile.flags = FS_INVALID;

	//Prepare Name Givin to FS Spicific
	int len = strlen(Find);
	len --;
	printf("len = %i\n", len);
	char *NAME = "          ";
	strcpy(NAME, Find);
	for(;len < 11; len ++)
		NAME[len] = 32;
	NAME[11] = 0;
	NAME = StringtoUpper(NAME);
	printf("Was Looking For ");
	printf("\"%s\"\n", NAME);
	for(int x = 0; x < 4096; x++)
		for(int y = 0; y < 16; y++) {
			//Check each file entry
			char tempNAME[12];
			strcpy(tempNAME, (const char*) Dir->Sector[x].DIRENT[y].Filename);
			tempNAME[11] = 0;
			if(streql(tempNAME, NAME)) {
				printf("WE FOUND IT!\n");
				tempFile.flags = ((Dir->Sector[x].DIRENT[y].Attrib & 0x10) ? (FS_DIRECTORY) : (FS_FILE));
				strcpy(tempFile.name, (const char*) Dir->Sector[x].DIRENT[y].Filename);
				tempFile.name[11] = (char)'\0';
				tempFile.fileLength = Dir->Sector[x].DIRENT[y].FileSize;
				tempFile.currentCluster = Dir->Sector[x].DIRENT[y].FirstCluster;
				tempFile.DiskID = DiskID;
				tempFile.FSysID = Get_FSysID(DiskID);
				return tempFile;
			}/* else {
				printf("\"%s\" != \"%s\"", tempNAME, NAME);
				(void) getch("");
			}*/

		}

	//This is just here for first run testings
	//memset(Dir, 0, sizeof(VFSDIR));
	return tempFile;
}

uint32_t _FAT_GetNextCluster(PFILE File)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line 126) _FAT_GetNextCluster(PFILE)\n\r");
#endif
	memset(&ClusterBuffer, 0, sizeof(ClusterBuffer));
	uint32_t NextCluster = 0;
	uint32_t CurrentCluster = File->currentCluster;
	uint32_t FATSector = Get_OffsetFatStart(File->DiskID);
	uint32_t ClustOffset = 0;
	if(File->FSysID == 0x1) {			//FAT12
		ClustOffset = CurrentCluster + (CurrentCluster / 2);
		FATSector += (ClustOffset / 512);
		if(!Read_Partition(Get_PartID(File->DiskID), &ClusterBuffer, FATSector, 2)) {
			uint16_t ClustTemp = *(uint16_t *) &ClusterBuffer[ClustOffset % 512];
			if(CurrentCluster & 1) {
				printf("\tODD CLUSTER = ");
				txf(1, "\tODD CLUSTER = ");
				NextCluster = (uint32_t) (ClustTemp >> 4);
			} else {
				printf("\tEVEN CLUSTER = ");
				txf(1, "\tEVEN CLUSTER = ");
				NextCluster = (uint32_t) (ClustTemp & 0xFFF);
			}
		}
	} else if(File->FSysID == 0x4) {	//FAT16
		ClustOffset = (CurrentCluster * 2);
		FATSector += (ClustOffset / 512);
		if(!Read_Partition(Get_PartID(File->DiskID), &ClusterBuffer, FATSector, 1)) {
			uint16_t ClustTemp = *(uint16_t *) &ClusterBuffer[ClustOffset % 512];
			NextCluster = (uint32_t) ClustTemp;
		}
	} else if(File->FSysID == 0xB || File->FSysID == 0xC) {	//FAT32
		ClustOffset = (CurrentCluster * 4);
		FATSector += (ClustOffset / 512);
		if(!Read_Partition(Get_PartID(File->DiskID), &ClusterBuffer, FATSector, 1)) {
			uint32_t ClustTemp = *(uint32_t *) &ClusterBuffer[ClustOffset % 512];
			NextCluster = ClustTemp & 0xFFFFFFF;
		}
	} else
		return 0;
	printf("\t%x\n", NextCluster);
	txf(1, "\t%x\n\r", NextCluster);
	return NextCluster;
}

void _FAT_Directory(PTDIR Dir, uint16_t DiskID, const char* Path)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line 172) _FAT_Directory(PTDIR, 0x%x, %s)\n\r", DiskID, Path);
#endif
	memset(Dir, 0, sizeof(TDIR));
	printf("Path = \"%s\"\n", Path);
	uint8_t FSID = Get_FSysID(DiskID);//1
	uint16_t PartID = Get_PartID(DiskID);//0
	if(!Path || Path == 0)
		return;
	else {
		char *ExpPath[100];
		int MAX = explode(ExpPath, Path, '/');
		for (int M = 0; M < MAX; M++)
			printf("\t%i = %s = %i\n", M, ExpPath[M], strlen(ExpPath[M]));

		for (int M = 0; M <= MAX; M++){
			if(!M) {
				uint16_t RCount = Get_RootCount(DiskID);//E0
				uint32_t RStart = Get_ROOTOFFSET(DiskID);//x13
				txf(1, "DEBUG A\n\r");
				//ROOTDIR
				if(RCount) { //Sector Based Root
					//FAT12/16
				txf(1, "DEBUG B\n\r");
				uint16_t tCount = RCount;
				tCount = (uint16_t) (tCount * 32);
				tCount = tCount / 512;
				txf(1, "DEBUG B2\n\r");
					if(Read_Partition(PartID, Dir, RStart, tCount )) {
						txf(1, "ERROR\n\r");
						return;
					}
				} else {	//Cluster Based Root
					FILE tempFile;
					tempFile.flags = FS_DIRECTORY;
					tempFile.currentCluster = RStart;
					tempFile.DiskID = DiskID;
					tempFile.FSysID = FSID;
					_FAT_Cluster_DIR(tempFile, Dir);
				}
			} if(M) {
				txf(1, "DEBUG C\n\r");
				if(strlen(ExpPath[M]) == 1)	{ // we are done
				txf(1, "DEBUG E\n\r");
					explodeFREE(ExpPath, MAX);
					return;
				}
				txf(1, "DEBUG F\n\r");
				//SUBDIR
				FILE tempFile;
				tempFile = _FAT_SearchDIR(Dir, (const char *) ExpPath[M], DiskID);
				if(tempFile.flags == FS_DIRECTORY)
					_FAT_Cluster_DIR(tempFile, Dir);
				else {
				txf(1, "DEBUG G\n\r");
					memset(Dir, 0, sizeof(VFSDIR));
					explodeFREE(ExpPath, MAX);
					return;
				}
			}
		}

		explodeFREE(ExpPath, MAX);
	}
}

void _FAT_DIR(uint16_t DiskID, PVFSDIR DIR, const char* Path)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line 230) _FAT_DIR(0x%x, PVFSDIR, %s)\n\r", DiskID, Path);
#endif
	if(Path && Path != 0) {
		_FAT_Directory(TempDIR, DiskID, Path);
		uint8_t FSysID = Get_FSysID(DiskID);
		int i = 0;
		for(int sectors = 0; sectors < 4096; sectors++)
			for(int Entry = 0; Entry < 16; Entry++) {
				if(TempDIR->Sector[sectors].DIRENT[Entry].Filename[0] == 0) // No More Entries?
					return; // Done Here
				if(TempDIR->Sector[sectors].DIRENT[Entry].Filename[0] != 0xE5 && TempDIR->Sector[sectors].DIRENT[Entry].Attrib != 0xF) {
					//Exists
					DIR->File[i].flags = ((TempDIR->Sector[sectors].DIRENT[Entry].Attrib & 0x10) ? (FS_DIRECTORY) : (FS_FILE));
					strcpy(DIR->File[i].name, (const char *) TempDIR->Sector[sectors].DIRENT[Entry].Filename);
					DIR->File[i].name[11] = (char)'\0';
					DIR->File[i].fileLength = TempDIR->Sector[sectors].DIRENT[Entry].FileSize;
					DIR->File[i].currentCluster = TempDIR->Sector[sectors].DIRENT[Entry].FirstCluster;
					DIR->File[i].DiskID = DiskID;
					DIR->File[i].FSysID = FSysID;
					i++;
				}
			}
	}
}

FILE _FAT_Open(uint16_t DiskID, const char* Path)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line --) _FAT_Open(0x%x, %s)\n\r", DiskID, Path);
#endif
	//
	FILE tempFile;
	tempFile.flags = FS_INVALID;
	return tempFile;
}

void _FAT_Close(PFILE File)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line --) _FAT_Close(PFILE)\n\r");
#endif
	if (File)
		File->flags = FS_INVALID;
}

void _FAT_Read(PFILE File, void *Buffer, uint32_t Length)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line --) _FAT_Read(PFILE 0x%x, *0x%x, 0x%x)\n\r", File, Buffer, Length);
#endif
	//
}

void _FAT_Write(PFILE File, void *Buffer, uint32_t Length)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line --) _FAT_Write(PFILE 0x%x, *0x%x, 0x%x)\n\r", File, Buffer, Length);
#endif
	//
}

error _FAT_MOUNT(PDISK Disk)
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line -291-) _FAT_MOUNT(PDISK Disk)\n\r");
#endif
	FAT32BS tempBS;
	memset(&tempBS, 0, sizeof(tempBS));
	if(!Read_Partition(Disk->PartID, &tempBS, 0, 1)) {
		if(tempBS.Bpb.SectorsPerCluster)
		{
			Disk->SectorsPerCluster = tempBS.Bpb.SectorsPerCluster;
			uint64_t DATAOFFSET = 0;
			DATAOFFSET += (uint64_t) tempBS.Bpb.ReservedSectors;
			Disk->FATOffset = (uint32_t) DATAOFFSET;
			if(tempBS.Bpb.SectorsPerFat)
				DATAOFFSET += (uint64_t) (((uint64_t) tempBS.Bpb.NumberOfFats * (uint64_t) tempBS.Bpb.SectorsPerFat) + (uint64_t) (( tempBS.Bpb.NumDirEntries * 32 ) / tempBS.Bpb.BytesPerSector));
			else
				DATAOFFSET += (uint64_t) ((uint64_t) tempBS.Bpb.NumberOfFats * (uint64_t) tempBS.BpbExt.SectorsPerFat32);
			Disk->OffsetDataStart = DATAOFFSET;
			printf("DATAOFFSET = 0x%x ", Disk->OffsetDataStart);
			printf("FATOFFSET = 0x%x ", Disk->FATOffset);
			printf("SectorsPerCluster = 0x%x\n", Disk->SectorsPerCluster);
			txf(1, "DATAOFFSET = 0x%x ", Disk->OffsetDataStart);
			txf(1, "FATOFFSET = 0x%x ", Disk->FATOffset);
			txf(1, "SectorsPerCluster = 0x%x\n\r", Disk->SectorsPerCluster);
			if(Disk->FSysID == 0x1 || Disk->FSysID == 0x4) { //FAT16/16
				Disk->ROOTOFFSET =  (uint32_t) (tempBS.Bpb.NumberOfFats * tempBS.Bpb.SectorsPerFat) + (uint32_t) tempBS.Bpb.ReservedSectors;
				Disk->RootCount = tempBS.Bpb.NumDirEntries;
			} else { //FAT32
				Disk->ROOTOFFSET = tempBS.BpbExt.RootCluster;
				Disk->RootCount = 0; // FAT32 is cluster mode.
			}
#ifdef DEBUG
	txf(1, "DATAOFFSET = 0x%x\t\tFATOFFSET = 0x%x\n\r", (uint64_t) Disk->OffsetDataStart, (uint64_t) Disk->FATOffset);
	txf(1, "\n\rBPB Info:\t");
	txf(1, "OEM Name\t%s\n\r", tempBS.Bpb.OEMName);
	txf(1, "BytesPerSector\t0x%x\t\tSectorsPerCluster\t0x%x\n\r", tempBS.Bpb.BytesPerSector, tempBS.Bpb.SectorsPerCluster);
	txf(1, "ReservedSectors\t0x%x\t\tNumberOfFats\t0x%x\n\r", tempBS.Bpb.ReservedSectors, tempBS.Bpb.NumberOfFats);
	txf(1, "RootDirEntries\t0x%x\t\tSectorsCount\t0x%x\n\r", tempBS.Bpb.NumDirEntries, tempBS.Bpb.NumSectors);
	txf(1, "Media\t0x%x\t\tSectorsPerFAT\t0x%x\n\r", tempBS.Bpb.Media, tempBS.Bpb.SectorsPerFat);
	txf(1, "SectorsPerTrack\t0x%x\t\tHeadsPerCyl\t0x%x\n\r", tempBS.Bpb.SectorsPerTrack, tempBS.Bpb.HeadsPerCyl);
	txf(1, "HiddenSectors\t0x%x\t\tLongSectors\t0x%x\n\r", tempBS.Bpb.HiddenSectors, tempBS.Bpb.LongSectors);
#endif
			return ERROR_NONE;
		} else
			return ERROR_NODATA;
	} else
		return ERROR_HWFAILURE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	  Public Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _FAT_init()
{
#ifdef DEBUG
	txf(1, "(FAT.C:Line -345-) _FAT_init()\n\r");
#endif
	TempDIR = calloc(0x200);
	FILESYSTEM FATFS;
	memset(&FATFS, 0, sizeof(FATFS));
	FATFS.DIR = &_FAT_DIR;
	FATFS.Open = &_FAT_Open;
	FATFS.Close = &_FAT_Close;
	FATFS.Read = &_FAT_Read;
	FATFS.Write = &_FAT_Write;
	FATFS.MOUNT = &_FAT_MOUNT;
	strcpy (FATFS.Name, "FAT12");
	Install_FS(FATFS, 0x1);
	strcpy (FATFS.Name, "FAT16");
	Install_FS(FATFS, 0x4);
	strcpy (FATFS.Name, "FAT32");
	Install_FS(FATFS, 0xB);
	Install_FS(FATFS, 0xC);
}
