#include "fat.h"
#include "fatcluster.h"

void ReadCluster(uint32_t PartID, uint32_t CurrentCluster, void* Buffer)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	uint32_t ClusterLength = (uint32_t) MyBPB->Bpb.SectorsPerCluster;
	uint32_t ClusterStart = 0;
	/*if(MyBPB->Bpb.SectorsPerFat)
		ClusterStart = (uint32_t) ((((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors) ) + ((CurrentCluster - 2) * ClusterLength) + ((MyBPB->Bpb.NumDirEntries * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector));
	else
		ClusterStart = (uint32_t) ((((MyBPB->BpbExt.SectorsPerFat32 * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors) ) + ((CurrentCluster - 2) * ClusterLength) + ((MyBPB->Bpb.NumDirEntries * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector));
	*/
	if(MyBPB->Bpb.SectorsPerFat)
		ClusterStart = (uint32_t) ((((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors)) + ((CurrentCluster - 2) * ClusterLength));
	else
		ClusterStart = (uint32_t) ((((MyBPB->BpbExt.SectorsPerFat32 * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors) ) + ((CurrentCluster - 2) * ClusterLength));
	if(MyBPB->Bpb.NumDirEntries)
		ClusterStart += (uint32_t) ((MyBPB->Bpb.NumDirEntries * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector);
	PartRead(PartID, (void*)ClusterStart, Buffer, ClusterLength);
#ifdef DEBUG_FULL
	DEBUG_printf("\nCluster = 0x%x, Start = 0x%x, Length = 0x%x\n", CurrentCluster, ClusterStart, ClusterLength);
	for(uint32_t x = 0; x < (ClusterLength*512); x++)
		DEBUG_printf("%x ", ((uint8_t*)Buffer)[x]);
	DEBUG_printf("\n");
#endif
}

void WriteCluster(uint32_t PartID, uint32_t CurrentCluster, void* Buffer)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	uint32_t ClusterLength = (uint32_t) (MyBPB->Bpb.SectorsPerCluster);
	uint32_t ClusterStart = 0;
	if(MyBPB->Bpb.SectorsPerFat)
		ClusterStart = (uint32_t) ((((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors)) + ((CurrentCluster - 2) * ClusterLength));
	else
		ClusterStart = (uint32_t) ((((MyBPB->BpbExt.SectorsPerFat32 * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors) ) + ((CurrentCluster - 2) * ClusterLength));
	if(MyBPB->Bpb.NumDirEntries)
		ClusterStart += (uint32_t) ((MyBPB->Bpb.NumDirEntries * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector);
	PartWrite(PartID, (void*)ClusterStart, Buffer, ClusterLength);
#ifdef DEBUG_FULL
	DEBUG_printf("\nCluster = 0x%x, Start = 0x%x, Length = 0x%x\n", CurrentCluster, ClusterStart, ClusterLength);
	for(uint32_t x = 0; x < (ClusterLength*512); x++)
		DEBUG_printf("%x ", ((uint8_t*)Buffer)[x]);
	DEBUG_printf("\n");
#endif
}

uint32_t NextCluster(uint32_t PartID, uint32_t CurrentCluster)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	uint32_t ret = 0;
	if(CurrentPartition->FSid == FAT12FSid) {
		//FAT12
		uint16_t *FAT = (uint16_t*)(CurrentPartition->FSdata->Buffer[1]);
		uint32_t fatoff = ((CurrentCluster >> 1) + CurrentCluster);
		uint16_t fattemp = *(uint16_t*)&CurrentPartition->FSdata->Buffer[1][fatoff];
		if(CurrentCluster & 1)
			ret = (uint32_t) (fattemp >> 4);
		else
			ret = (uint32_t) (fattemp & 0x0FFF);
		if (ret >= 0xFF0) 
			ret = 0xFFFFFFFF;
	} else if(CurrentPartition->FSid == FAT16FSid) {
		//FAT16
		uint16_t *FAT = (uint16_t*)(CurrentPartition->FSdata->Buffer[1]);
		ret = (uint32_t) FAT[CurrentCluster];
		if (ret >= 0xFFF0) 
			ret = 0xFFFFFFFF;
	} else {
		//FAT32
		uint32_t *FAT = (uint32_t*)(CurrentPartition->FSdata->Buffer[1]);
		ret = FAT[CurrentCluster];
		if (ret >= 0xFFFFFFF0) 
			ret = 0xFFFFFFFF;
	}
	return ret;
}

uint32_t CountCluster(uint32_t PartID, uint32_t CurrentCluster)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t x = 1;
	uint32_t Clust = CurrentCluster;

	while((Clust = NextCluster(PartID, Clust)) != 0xFFFFFFFF)
		x++;

	return x;
}


uint32_t CountClusterChain(uint32_t PartID, uint32_t CurrentCluster)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t x = 1;
	uint32_t Clust = 0;
	uint32_t prevClust = CurrentCluster;

	while((Clust = NextCluster(PartID, prevClust)) != 0xFFFFFFFF) {
		if(Clust != (prevClust + 1))
			return x;
		x++;
		prevClust = Clust;
	}

	return x;
}

void ReadClusterChain(uint32_t PartID, uint32_t CurrentCluster, void* Buffer, uint32_t Count)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	uint32_t ClusterLength = (uint32_t) (MyBPB->Bpb.SectorsPerCluster);
	uint32_t ClusterStart = 0;
	if(MyBPB->Bpb.SectorsPerFat)
		ClusterStart = (uint32_t) ((((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors) ) + ((CurrentCluster - 2) * ClusterLength) + ((MyBPB->Bpb.NumDirEntries * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector));
	else
		ClusterStart = (uint32_t) ((((MyBPB->BpbExt.SectorsPerFat32 * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors) ) + ((CurrentCluster - 2) * ClusterLength) + ((MyBPB->Bpb.NumDirEntries * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector));
	ClusterLength *= Count;
	PartRead(PartID, (void*)ClusterStart, Buffer, ClusterLength);
#ifdef DEBUG_FULL
	DEBUG_printf("\nCluster = 0x%x, Start = 0x%x, Length = 0x%x\n", CurrentCluster, ClusterStart, ClusterLength);
	for(uint32_t x = 0; x < (ClusterLength * 512); x++)
		DEBUG_printf("%x ", ((uint8_t*)Buffer)[x]);
	DEBUG_printf("\n");
#endif
}

uint32_t FindFreeCluster(uint32_t PartID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	uint32_t ret = 3;
	uint32_t FATEntries = 0;
	if(CurrentPartition->FSid == FAT12FSid)
		FATEntries = (uint32_t) ((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.BytesPerSector) / 12);
	else if (CurrentPartition->FSid == FAT16FSid)
		FATEntries = (uint32_t) ((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.BytesPerSector) / 16);
	else
		FATEntries = (uint32_t) ((MyBPB->BpbExt.SectorsPerFat32 * MyBPB->Bpb.BytesPerSector) / 32);
	while(NextCluster(PartID, ret) && (ret < FATEntries))
		ret++;
	if(ret == FATEntries)
		return 0xFFFFFFFF;
	return ret;
}

void AllocateCluster(uint32_t PartID, uint32_t CurrentCluster)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	if(CurrentPartition->FSid == FAT12FSid) {
		//FAT12
		uint32_t fatoff = ((CurrentCluster >> 1) + CurrentCluster);
		uint16_t *FAT = (uint16_t*)&CurrentPartition->FSdata->Buffer[1][fatoff];
		// uint16_t fattemp = *(uint16_t*)&CurrentPartition->FSdata->Buffer[1][fatoff];
		if(CurrentCluster & 1) {
			FAT[0] &= 0x000F;
			FAT[0] |= 0xFFF0;
		} else {
			FAT[0] &= 0xF000;
			FAT[0] |= 0x0FFF;
		}
		// FAT[0] = fattemp;
		// FAT[fatoff] = (uint8_t) (fattemp & 0xFF);
		// FAT[fatoff + 1] = (uint8_t) (fattemp >> 8);
	} else if(CurrentPartition->FSid == FAT16FSid) {
		//FAT16
		uint16_t *FAT = (uint16_t*)(CurrentPartition->FSdata->Buffer[1]);
		FAT[CurrentCluster] = 0xFFFF;
	} else {
		//FAT32
		uint32_t *FAT = (uint32_t*)(CurrentPartition->FSdata->Buffer[1]);
		FAT[CurrentCluster] = 0xFFFFFFFF;
	}
}

void LinkCluster(uint32_t PartID, uint32_t Cluster0, uint32_t Cluster1)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	if(CurrentPartition->FSid == FAT12FSid) {
		//FAT12
		uint32_t fatoff = ((Cluster0 >> 1) + Cluster0);
		uint16_t *FAT = (uint16_t*)&CurrentPartition->FSdata->Buffer[1][fatoff];
		// uint16_t fattemp = *(uint16_t*)&CurrentPartition->FSdata->Buffer[1][fatoff];
		if(Cluster0 & 1) {
			FAT[0] &= 0x000F;
			FAT[0] |= (uint16_t) (Cluster1 << 4);
		} else {
			FAT[0] &= 0xF000;
			FAT[0] |= (uint16_t) (Cluster1 & 0x0FFF);
		}
		// FAT[0] = fattemp;
	} else if(CurrentPartition->FSid == FAT16FSid) {
		//FAT16
		uint16_t *FAT = (uint16_t*)(CurrentPartition->FSdata->Buffer[1]);
		FAT[Cluster0] = (uint16_t) Cluster1;
	} else {
		//FAT32
		uint32_t *FAT = (uint32_t*)(CurrentPartition->FSdata->Buffer[1]);
		FAT[Cluster0] = Cluster1;
	}
}

void ReadFAT(uint32_t PartID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	uint32_t FATStart = 0, FATLength = 0;
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	if(!MyBPB->Bpb.SectorsPerFat)															//FAT32
		FATLength = (uint32_t) (MyBPB->BpbExt.SectorsPerFat32);
	else																					//FAT12/16
		FATLength = (uint32_t) (MyBPB->Bpb.SectorsPerFat);
	
	FATStart = (uint32_t) (MyBPB->Bpb.ReservedSectors);
	if(!CurrentPartition->FSdata->Buffer[1])
		CurrentPartition->FSdata->Buffer[1] = calloc(((uint32_t)(FATLength * (uint32_t)MyBPB->Bpb.BytesPerSector)));							// Initilize Buffer If Not Allocated
	PartRead(PartID, (void*) FATStart, CurrentPartition->FSdata->Buffer[1], FATLength);
}

void WriteFAT(uint32_t PartID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	if(CurrentPartition->FSdata->Buffer[1]) {
		uint32_t FATStart = 0, FATLength = 0;
		PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
		if(!MyBPB->Bpb.SectorsPerFat)														//FAT32
			FATLength = (uint32_t) (MyBPB->BpbExt.SectorsPerFat32);
		else																				//FAT12/16
			FATLength = (uint32_t) (MyBPB->Bpb.SectorsPerFat);
		
		FATStart = (uint32_t) (MyBPB->Bpb.ReservedSectors);
		for(uint32_t x = 0; x < (uint32_t) MyBPB->Bpb.NumberOfFats; x++)
			PartWrite(PartID, (void*) (FATStart + (x * FATLength)), CurrentPartition->FSdata->Buffer[1], FATLength);
	}
}