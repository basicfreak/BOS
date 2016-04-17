/*
NOTE: This driver is not finished and is only a simi-functional place holder!
*/

#include "fat.h"
#include <stddef.h>

static BPB_p BPB = (BPB_p) 0x7C00;
static void* FAT_BUFFER = (void*) 0x10000;
static void* ROOT_BUFFER = (void*) 0x20000;
static char PathBuffer[1024] = { 0 };
static int isFATINITd = 0;
static uint32_t CurrentFile = 0;
static uint32_t CurrentCluster = 0;
static uint32_t DataStart = 0;
static char FATTYPE = 0;

int FAT_Open(void* Path)
{
	if(!isFATINITd)
		FAT_INIT();

	int depth = ConvertPath((char*)PathBuffer, (char*)Path);

	if(!depth)
		return 2;
	uint32_t FileID = FindFile((char*)PathBuffer, depth);
	if(FileID) {
		CurrentFile = FileID;
		CurrentCluster = FileID;
		return 0;
	}
	return 6;
}

int FAT_Read(void* Buffer)
{
	if(!CurrentFile)
		return 5;
	CurrentCluster = CurrentFile;
	int FileSize = ReadCluster(Buffer, CurrentCluster);
	while ((CurrentCluster = NextCluster(CurrentCluster))) {
		FileSize += ReadCluster((void*)((size_t)Buffer + (size_t)FileSize), CurrentCluster);
	}
	if(FileSize)
		return 0;
	return 3;
}

int FAT_Write(void* Buffer, int Offset, int size)
{
	if(!CurrentFile)
		return 5;
	return 3;
}

int FAT_Close()
{
	if(!CurrentFile)
		return 5;
	CurrentFile = 0;
	return 0;
}

static void FAT_INIT()
{
	DataStart = (BPB->HiddenSectors + BPB->ReservedSectors);
	if(BPB->RootDirEnts) {
		if((!BPB->Sectors) || ((BPB->Sectors / BPB->SectorsPerCluster) > 0x0FF0)) {
			//FAT 16
			FATTYPE = 16;
		} else {
			//FAT 12
			FATTYPE = 12;
		}
		DataStart += (uint32_t)(BPB->FATsectors * BPB->FATs);
		DataStart += (uint32_t)(BPB->RootDirEnts / (BPB->BytesPerSector / 32));
		if((BPB->RootDirEnts * 32 <= 0x10000)) ReadSectors_Safe(ROOT_BUFFER, (int)(BPB->ReservedSectors + BPB->HiddenSectors + (uint32_t)(BPB->FATs * BPB->FATsectors)), (int)(BPB->RootDirEnts * 32 / BPB->BytesPerSector));
		else ReadSectors_Safe(ROOT_BUFFER, (int)(BPB->ReservedSectors + BPB->HiddenSectors + (uint32_t)(BPB->FATs * BPB->FATsectors)), (int)(0x10000 / BPB->BytesPerSector));
	} else {
		// FAT 32
		FATTYPE = 32;
		DataStart += (BPB->BigFATsectors * BPB->FATs);
		int Offset = ReadCluster(ROOT_BUFFER, BPB->RootCluster);
		uint32_t Cluster = BPB->RootCluster;
		while ((Cluster = NextCluster(Cluster)) && (Offset < 0x10000)) {
			Offset += ReadCluster((void*)((size_t)ROOT_BUFFER + (size_t)Offset), Cluster);
		}
	}
	// Read FAT into Buffer (upto 64KB)
	int count = 0;
	if(BPB->FATsectors)
		count = (int)BPB->FATsectors;
	else
		count = (int)BPB->BigFATsectors;
	if((count * BPB->BytesPerSector) > 0x10000)
		count = (int)(0x10000 / BPB->BytesPerSector);
	CurrentCluster = 0;
	ReadSectors_Safe(FAT_BUFFER, (int)(BPB->HiddenSectors + BPB->ReservedSectors), count);
	isFATINITd = 1;
	return;
}

static int ReadSectors_Safe(void* Buffer, int Offset, int Count)
{
	int loops = 0;
	int ret = 0;
	if(Count * BPB->BytesPerSector / 0x4000)
		for(; loops < (Count * BPB->BytesPerSector / 0x4000); loops++)
			ret += ReadSectors((void*)(((size_t)Buffer) + (size_t)(loops * 0x4000)), (Offset + (loops * 0x4000 / BPB->BytesPerSector)), (0x4000 / BPB->BytesPerSector));
	if((Count * BPB->BytesPerSector) % 0x4000)
		ret += ReadSectors((void*)(((size_t)Buffer) + (size_t)(loops * 0x4000)), (Offset + (loops * 0x4000 / BPB->BytesPerSector)), (((Count * BPB->BytesPerSector) % 0x4000) / BPB->BytesPerSector));
	return ret;
}

static uint32_t NextCluster(uint32_t Cluster)
{
	uint32_t ret = 0;
	if (FATTYPE == 12) {
		uint16_t *FAT = (uint16_t*) FAT_BUFFER;
		uint16_t tmp = FAT[((Cluster >> 1) + Cluster)];
		if(Cluster & 1)
			ret = (uint32_t) (tmp >> 4);
		else
			ret = (uint32_t) (tmp & 0x0FFF);
		if (ret >= 0xFF6)
			ret = 0;
	} else if (FATTYPE == 16) {
		uint16_t *FAT = (uint16_t*) FAT_BUFFER;
		ret = (uint32_t) FAT[Cluster];
		if(ret >= 0xFFF6)
			ret = 0;
	} else {
		uint32_t *FAT = (uint32_t*) FAT_BUFFER;
		ret = FAT[Cluster];
		if(ret >= 0xFFFFFF6)
			ret = 0;
	}
	return ret;
}

static int ReadCluster(void* Buffer, uint32_t Cluster)
{
	int Offset = (int)DataStart;
	Offset += (int)((Cluster - 2) * BPB->SectorsPerCluster);
	return ReadSectors_Safe(Buffer, Offset, BPB->SectorsPerCluster);
}

static int ConvertPath(char* Out, char* In)
{
	int depth = 1;
	int InLength = 0;
	int OutLength = 0;
	char* temp = In;
	while(*temp++)
		InLength++;
	for(int i = 0; i < InLength; i++) {
		if((In[i] == '/')) {
			if(i) {
				while(OutLength % 11)
					Out[OutLength++] = ' ';
				depth++;
			}
		} else if(In[i] == '.') {
			if((OutLength % 11) > 8)
				return 0;
			while((OutLength % 11) % 8)
				Out[OutLength++] = ' ';
		} else if(In[i] == (char)0xE5) {
			Out[OutLength++] = 5;
		} else {
			if(In[i] >= 'a' && In[i] <= 'z')
				Out[OutLength++] = (char)(In[i] - ' ');
			else
				Out[OutLength++] = In[i];
			if(!(OutLength % 11) && ((In[i + 1] != '/') && (In[i+1] != 0)))
				return 0;
		}
	}
	while((Out[OutLength] % 11))
		Out[OutLength++] = ' ';
	Out[OutLength] = 0;
	return depth;
}

static uint32_t FindFile(char* Path, int Depth)
{
	uint32_t FileID = 0;
	uint32_t DirID = 0;
	for(int i = 0; i < Depth; i++) {
		if(!FileID) {
			FileID = SearchDir(ROOT_BUFFER, Path, 2048);
		} else {
			DirID = FileID;
			int ClusterSize = ReadCluster((void*)0x1000, DirID);
			FileID = SearchDir((void*)0x1000, (char*)&Path[(i*11)], (ClusterSize / 32));
			while(!FileID && (DirID = NextCluster(DirID))) {
				ClusterSize = ReadCluster((void*)0x1000, DirID);
				FileID = SearchDir((void*)0x1000, (char*)&Path[(i*11)], (ClusterSize / 32));
			}
			if(!FileID)
				return 0;
		}
	}
	return FileID;
}

static uint32_t SearchDir(void* Buffer, char* FileName, int Entries)
{
	DIR_p Dir = (DIR_p) Buffer;
	for(int i = 0; i < Entries; i++) {
		int match = 1;
		for(int x = 0; x < 11; x++) {
			if(Dir[i].FileName[x] != FileName[x])
				match = 0;
		}
		if(match) {
			return (uint32_t) ((Dir[i].Cluster_High << 16) | Dir[i].Cluster_Low);
		}
	}
	return 0;
}
