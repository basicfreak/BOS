// ./SRC/MAIN.C

#include <BOSBOOT.h>
#include "fat.h"
#include "fatcluster.h"
#include "fatdir.h"

void directory(FILE *dir);
void newFile(FILE *file, char** path);
void open(FILE *file, char** path);
void read(FILE *file, void* Address, void* Buffer, uint32_t Length);
void write(FILE *file, void* Address, void* Buffer, uint32_t Length);
void mount(uint32_t PartID);
void umount(uint32_t PartID);

bool InitilizeFSDATA(uint32_t PartID);

int init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	FS_p MyFS0 = (FS_p) calloc(sizeof(FS_t));
	FS_p MyFS1 = (FS_p) calloc(sizeof(FS_t));
	FS_p MyFS2 = (FS_p) calloc(sizeof(FS_t));
	MyFS0->directory = directory;
	MyFS1->directory = directory;
	MyFS2->directory = directory;
	MyFS0->newFile = newFile;
	MyFS1->newFile = newFile;
	MyFS2->newFile = newFile;
	MyFS0->open = open;
	MyFS1->open = open;
	MyFS2->open = open;
	MyFS0->read = read;
	MyFS1->read = read;
	MyFS2->read = read;
	MyFS0->write = write;
	MyFS1->write = write;
	MyFS2->write = write;
	MyFS0->mount = mount;
	MyFS1->mount = mount;
	MyFS2->mount = mount;
	MyFS0->umount = umount;
	MyFS1->umount = umount;
	MyFS2->umount = umount;
	strcpy((uint8_t*)MyFS0->name, (const uint8_t*)"FAT12");
	strcpy((uint8_t*)MyFS1->name, (const uint8_t*)"FAT16");
	strcpy((uint8_t*)MyFS2->name, (const uint8_t*)"FAT32");
	mkFileSystem(FAT12FSid, MyFS0);
	mkFileSystem(FAT16FSid, MyFS1);
	mkFileSystem(FAT32FSid, MyFS2);
	mkFileSystem(FAT32FS2id, MyFS2);

	/*InitilizeFSDATA(0);*/
 	/*CreateDir(0, 0, "TESTDIR    ");
 	FILE *MyDir = fopen("/FDC/testdir");
 	DEBUG_printf("TESTDIR Start = 0x%x (%i)\n", MyDir->Start, MyDir->Start);
	sleep(2);
	if(MyDir->Start) {
		CreateDir(0, MyDir->Start, "TESTSUBD   ");
		fclose(MyDir);
	 }*/
	/*fnewDIR("/FDC/test");
	fnewDIR("/FDC/test/bin");
	FILE *MyFile = fnew("/FDC/test/bin/pci.dll");
	DEBUG_printf("\nNew File's Start Cluster = 0x%x (%i)!\n", MyFile->Start, MyFile->Start);
	if(MyFile->Start) {
		FILE *InFile = fopen("/FDC/bin/pci.dll");
		if(InFile->Start) {
			void *TempBuffer = calloc(InFile->Length);
			fread(InFile, 0, TempBuffer, InFile->Length);
			fwrite(MyFile, 0, TempBuffer, InFile->Length);
			free(TempBuffer, sizeof(InFile->Length));
		}
		fclose(InFile);
	}
	fclose(MyFile);*/
	/*FILE *fnew(char* path);
	void fnewDIR(char* path);*/

	/*FILE *MyFile;
	void *MyFileBuff;
	
	MyFile = fopen("/FDC/bin/pci.dll");
	MyFileBuff = calloc(MyFile->Length);

	fread(MyFile, 0, MyFileBuff, MyFile->Length);
	fclose(MyFile);

	MyFile = fopen("/FDC/bin/pci.dll");

	fwrite(MyFile, 0, MyFileBuff, MyFile->Length);
	free(MyFileBuff, MyFile->Length);
	fclose(MyFile);*/

	/*FILE *VGAMOD = fopen("/FDC/test/bin/pci.dll");
DEBUG_printf("MyFile Length = %i\tStart = 0x%x\n", VGAMOD->Length, VGAMOD->Start);
	void* VGAMODBUFF = calloc(VGAMOD->Length);
	fread(VGAMOD, 0, VGAMODBUFF, VGAMOD->Length);*/
/*
DEBUG_printf("LOADING VGA MOD TEST\n");
for(uint32_t x = 0; x < (800*600*2/0x1000) + 1; x ++)
	_VMM_map((BOOTINF.FrameBuffer + (x * 0x1000)), (0x800000 + (x * 0x1000)), TRUE, TRUE);*/
	/*BOOTINF.FrameBuffer = 0x800000;
	Load_Mod(VGAMODBUFF);

	free(VGAMODBUFF, VGAMOD->Length);
	fclose(VGAMOD);

 	for(;;);*/

	return 0;
}

void directory(FILE *dir)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	//
}
/*
	FILE *fnew(char* path);
	void fnewDIR(char* path)
	; should give me **path without the last entry, and a FILE* with info on file/dir type*/
void newFile(FILE *file, char** path)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(file->PartID);
	if(!CurrentPartition->FSdata)
		mount(file->PartID);
	if(!CurrentPartition->FSdata)
		return;
	
	uint32_t ParrentDir = 0;
	if(path[2]) {
		FILE *LocalFile = (FILE*) calloc(sizeof(FILE));
		LocalFile->PartID = file->PartID;
		open(LocalFile, path);
		// Folder Start Cluster should be LocalFile->Start...
		ParrentDir = LocalFile->Start;
		free(LocalFile, sizeof(FILE));
	}
	char *DOSFName = (char*) calloc(12);
	ConvertFName((volatile char*) DOSFName, (const char*) file->Name);
DEBUG_printf("DOSFName = \"%s\"\n", DOSFName);
	if(file->Flags & FF_DIR) {
		// Create A Directory (I Have A Function For That ;) [fatdir.h]
		CreateDir(file->PartID, ParrentDir, DOSFName);
	} else {
		// Create Blank File
		uint32_t NewCluster = FindFreeCluster(file->PartID); // Find Free Cluster
		AllocateCluster(file->PartID, NewCluster); // Allocate Cluster
		WriteFAT(file->PartID);
		PFATDIR NewDirEnt = (PFATDIR) calloc(sizeof(FATDIR));
		memcpy(NewDirEnt->Filename, DOSFName, 11);
		NewDirEnt->FirstClusterHiBytes = (uint16_t) (NewCluster >> 16);
		NewDirEnt->FirstCluster = (uint16_t) (NewCluster & 0xFFFF);
		addDirEnt(file->PartID, NewDirEnt, ParrentDir);
		free(NewDirEnt, sizeof(FATDIR));
		memcpy(file->Name, DOSFName, 11);
		file->Start = NewCluster;
		file->PreviousEntry = (void*)ParrentDir;
	}
	free(DOSFName, 12);
}

void open(FILE *file, char** path)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(file->PartID);
	if(!CurrentPartition->FSdata)
		mount(file->PartID);
	if(!CurrentPartition->FSdata)
		return;

	int x = 2;
	PFATDIR MyDirEntry = (PFATDIR)calloc(sizeof(FATDIR));
	PFATDIR ParrentDirEntry = (PFATDIR)calloc(sizeof(FATDIR));
	while(path[x]) {     // void FindDirEntry(volatile PFATDIR DirEntry, char FName[11]);
		char *DOSFName = (char*) calloc(12);
		ConvertFName((volatile char*) DOSFName, (const char*) path[x]);
		SearchDir(file->PartID, (volatile PFATDIR) MyDirEntry, DOSFName);
		free(DOSFName, 12);
		x++;
		if(path[x])
			memcpy(ParrentDirEntry, MyDirEntry, sizeof(FATDIR));
	}

	memcpy(file->Name, MyDirEntry->Filename, 11);
	file->Name[12] = 0;
	file->Start = (uint32_t) ((MyDirEntry->FirstClusterHiBytes << 16) | MyDirEntry->FirstCluster);
	file->Length = (uint32_t) MyDirEntry->FileSize;
	file->Flags = 0;

	file->PreviousEntry = (void*) ((uint32_t) (ParrentDirEntry->FirstClusterHiBytes << 16 | ParrentDirEntry->FirstCluster));
#ifdef DEBUG_FULL
DEBUG_printf("\n");
for(uint32_t xyz = 0; xyz < 32; xyz++)
	DEBUG_printf("%x ", ((uint8_t*)MyDirEntry)[xyz]);
DEBUG_printf("\n");
#endif
	free(ParrentDirEntry, sizeof(FATDIR));
	free(MyDirEntry, sizeof(FATDIR));
}

void read(FILE *file, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t PartID = file->PartID;
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	uint32_t ClusterLength = (uint32_t) (MyBPB->Bpb.SectorsPerCluster);

	uint32_t FileOffset = (uint32_t) ((Address) ? (Address) : (file->Offset)); //if address is set assume it as offset from beginning
	uint32_t ClusterNumber = (FileOffset / (ClusterLength  * MyBPB->Bpb.BytesPerSector));
	uint32_t ClusterOffset = (FileOffset % (ClusterLength  * MyBPB->Bpb.BytesPerSector));
	uint32_t ClusterCount = ((ClusterOffset + Length) / (ClusterLength  * MyBPB->Bpb.BytesPerSector));
	if(((ClusterOffset + Length) % (ClusterLength  * MyBPB->Bpb.BytesPerSector)))
		ClusterCount++;

	uint32_t CurrentCluster = file->Start;

	void* TempData = calloc((ClusterCount * ClusterLength ) * MyBPB->Bpb.BytesPerSector);

	while(ClusterNumber-- && CurrentCluster != 0xFFFFFFFF)
		CurrentCluster = NextCluster(PartID, CurrentCluster);

	uint32_t ClusterCountTemp = ClusterCount;
	uint32_t x = 0;

	while(ClusterCountTemp && CurrentCluster != 0xFFFFFFFF) {
		uint32_t TempCount = CountClusterChain(PartID, CurrentCluster);
		if(TempCount >= ClusterCountTemp) {
			ReadClusterChain(PartID, CurrentCluster, (void*)((uint32_t*)TempData + (x * (ClusterLength  * MyBPB->Bpb.BytesPerSector))), ClusterCountTemp);
			ClusterCountTemp = 0;
			x = ClusterCount;
			CurrentCluster = NextCluster(PartID, CurrentCluster + ClusterCountTemp - 1);
		} else {
			ReadClusterChain(PartID, CurrentCluster, (void*)((uint32_t*)TempData + (x * (ClusterLength  * MyBPB->Bpb.BytesPerSector))), TempCount);
			CurrentCluster = NextCluster(PartID, CurrentCluster + (TempCount - 1));
			ClusterCountTemp -= (TempCount);
			x += TempCount;
		}
	}

	memcpy(Buffer, (void*) ((uint32_t)TempData + ClusterOffset), Length);

	file->Offset = (FileOffset + Length);

	free(TempData, (ClusterCount * (ClusterLength  * MyBPB->Bpb.BytesPerSector)));
}

void write(FILE *file, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	bool MODIFIED_FAT = FALSE;
	uint32_t PartID = file->PartID;
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	uint32_t ClusterLength = (uint32_t) (MyBPB->Bpb.SectorsPerCluster);

	uint32_t FileOffset = (uint32_t) ((Address) ? (Address) : (file->Offset)); //if address is set assume it as offset from beginning
	uint32_t ClusterNumber = (FileOffset / (ClusterLength * MyBPB->Bpb.BytesPerSector));
	uint32_t ClusterOffset = (FileOffset % (ClusterLength * MyBPB->Bpb.BytesPerSector));
	uint32_t ClusterCount = ((ClusterOffset + Length) / (ClusterLength * MyBPB->Bpb.BytesPerSector));
	if(((ClusterOffset + Length) % (ClusterLength * MyBPB->Bpb.BytesPerSector)))
		ClusterCount++;

	uint32_t CurrentCluster = file->Start;

	void* TempData = calloc((ClusterCount * ClusterLength )* MyBPB->Bpb.BytesPerSector);

	while(ClusterNumber--) { // && CurrentCluster != 0xFFFFFFFF) {
		if(ClusterNumber && (NextCluster(PartID, CurrentCluster) == 0xFFFFFFFF)) {
			// Note we are writting past last known cluster do something about it!
			uint32_t NewCluster = FindFreeCluster(PartID);
			AllocateCluster(PartID, NewCluster);
			LinkCluster(PartID, CurrentCluster, NewCluster);
			WriteCluster(PartID, NewCluster, TempData);
			MODIFIED_FAT = TRUE;
		}
		CurrentCluster = NextCluster(PartID, CurrentCluster);
	}

	uint32_t ClusterCountTemp = ClusterCount;
	uint32_t x = 0;

	if(ClusterOffset) //Partial first cluster write! Read First!!!!!
		ReadCluster(PartID, CurrentCluster, TempData);
	if(((ClusterOffset + Length) % ClusterLength)) { //Partial Last Cluster Write! Read First!!!!!
		uint32_t lastCluster = CurrentCluster;
		uint32_t ClustCountTemp = ClusterCount;
		while(ClustCountTemp-- && lastCluster != 0xFFFFFFFF)
			lastCluster = NextCluster(PartID, lastCluster);
		if(lastCluster != 0xFFFFFFFF) {
			ReadCluster(PartID, lastCluster, (void*)(((uint32_t) TempData) + ((ClusterCount - 1) * (ClusterLength  * MyBPB->Bpb.BytesPerSector))));
		}
	}

	memcpy((void*)((uint32_t)TempData + ClusterOffset), Buffer, Length);

	while(ClusterCountTemp && CurrentCluster != 0xFFFFFFFF) {
		WriteCluster(PartID, CurrentCluster, (void*)((uint32_t)TempData + ((ClusterCount - ClusterCountTemp) * (ClusterLength  * MyBPB->Bpb.BytesPerSector))));
		ClusterCountTemp--;
		if(ClusterCountTemp && (NextCluster(PartID, CurrentCluster) == 0xFFFFFFFF)) {
			// Note we are writting past last known cluster do something about it!
			uint32_t NewCluster = FindFreeCluster(PartID);
			AllocateCluster(PartID, NewCluster);
			LinkCluster(PartID, CurrentCluster, NewCluster);
			MODIFIED_FAT = TRUE;
		}
		CurrentCluster = NextCluster(PartID, CurrentCluster);
	}

	file->Offset = (FileOffset + Length);

	if(file->Offset > file->Length) {
		file->Length = file->Offset;
		PFATDIR MyDirEntry = (PFATDIR) calloc(sizeof(FATDIR));

		memcpy(MyDirEntry->Filename, file->Name, 11);
		MyDirEntry->FileSize = file->Length;
		MyDirEntry->FirstCluster = (uint16_t) (file->Start & 0xFFFF);
		MyDirEntry->FirstClusterHiBytes = (uint16_t) (file->Start >> 16);
		MyDirEntry->Attrib = ((file->Flags & FF_DIR) ? (FAT_ATTRIB_DIR) : (0));
		UpdateDirEntry(PartID, MyDirEntry, (uint32_t) file->PreviousEntry);
		free(MyDirEntry, sizeof(FATDIR));
	}

	free(TempData, (ClusterCount * (ClusterLength  * MyBPB->Bpb.BytesPerSector)));
	if(MODIFIED_FAT)
		WriteFAT(PartID);
}

void mount(uint32_t PartID)
{
	if(InitilizeFSDATA(PartID)) {
		// Continue Mounting if we initilized data
		PART_p CurrentPartition = getPartition(PartID);
		if((CurrentPartition->FSid == FAT32FS2id) || (CurrentPartition->FSid == FAT32FSid)) {
			PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
			memcpy(CurrentPartition->Name, MyBPB->BpbExt.VolumeLable, 11);
		} else {
			PFAT1216BS MyBPB = (PFAT1216BS) CurrentPartition->FSdata->Buffer[0];
			memcpy(CurrentPartition->Name, MyBPB->BpbExt.VolumeLable, 11);
		}

	}
}

void umount(uint32_t PartID)
{
	PART_p CurrentPartition = getPartition(PartID);
	memset(CurrentPartition->Name, 0, 11);
	if(CurrentPartition->FSdata) {
		PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
		uint32_t FATLength = 0, RootLength = 0;
		if(MyBPB->Bpb.NumDirEntries)
			RootLength = (uint32_t) (MyBPB->Bpb.NumDirEntries * sizeof(FATDIR));
		else
			RootLength = (CountCluster(PartID, MyBPB->BpbExt.RootCluster) * ((uint32_t) (MyBPB->Bpb.SectorsPerCluster)));
		if(!MyBPB->Bpb.SectorsPerFat)															//FAT32
			FATLength = (uint32_t) (MyBPB->BpbExt.SectorsPerFat32);
		else																					//FAT12/16
			FATLength = (uint32_t) (MyBPB->Bpb.SectorsPerFat);
		RootLength *= MyBPB->Bpb.BytesPerSector;
		FATLength *= MyBPB->Bpb.BytesPerSector;
		

		free(CurrentPartition->FSdata->Buffer[2], RootLength);
		free(CurrentPartition->FSdata->Buffer[1], FATLength);
		free(CurrentPartition->FSdata->Buffer[0], 512);
		free(CurrentPartition->FSdata, sizeof(FSDATA_t));
	}
}

bool InitilizeFSDATA(uint32_t PartID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	if(!CurrentPartition->FSdata) // Do we have the Buffer Table?
		CurrentPartition->FSdata = (FSDATA_p) calloc(sizeof(FSDATA_t));
	if(!CurrentPartition->FSdata->Buffer[0]) { // Do we have BPB Initilized?
		CurrentPartition->FSdata->Buffer[0] = calloc(0x200);
		PartRead(PartID, 0, CurrentPartition->FSdata->Buffer[0], 1);
	}
	if(!(((PFAT1216BS)CurrentPartition->FSdata->Buffer[0])->Bpb.BytesPerSector)) {
		// Disk Error...
		free(CurrentPartition->FSdata->Buffer[0], 0x200);
		free(CurrentPartition->FSdata, sizeof(FSDATA_t));
		return FALSE;
	}
	ReadFAT(PartID);
	ReadRootDir(PartID);
	return TRUE;
}
