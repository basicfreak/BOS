#include "fatdir.h"
#include "fatcluster.h"

void ConvertFName(volatile char* output, const char* input)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	size_t MyLength = (strlen(input) - 1);
	size_t AtLength = 0;
	if(MyLength > 11) // NO LFN Support Here Yet
		return;
	for(size_t x = 0; x < 8; x++) {
		if(input[AtLength] == '.' || AtLength >= MyLength)
			output[x] = (char)' ';
		else
			output[x] = input[AtLength++];
	}
	if(input[AtLength] == '.')
		AtLength++;
	for(size_t x = 8; x < 11; x++) {
		if(AtLength >= MyLength)
			output[x] = (char)' ';
		else
			output[x] = input[AtLength++];
	}
	for(int x = 0; x < 11; x++)
		if(output[x] >= 'a' || output[x] <= 'z')
			output[x] -= (char)' ';
	for(int x = 0; x < 11; x++)
		if(output[x] == 0)
			output[x] = (char)' ';
#ifdef DEBUG
	DEBUG_printf("DOS File Name = \"%s\"\n", output);
#endif
}

bool CompareFName(const char* s1, const char* s2)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	for(int x = 0; x < 11; x++)
		if(s1[x] != s2[x])
			return FALSE;
	return TRUE;
}

void SearchDir(uint32_t PartID, volatile PFATDIR MyDirEntry, char* FName)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	uint32_t CurrentCluster = MyDirEntry->FirstCluster;
	uint32_t RootEntries = 0;
	uint32_t x = 0;
	if(!CurrentCluster) {
		// This is root DIR or an error but we assume no error for now...
		uint32_t DIREnts = 0;
		//uint32_t CountCluster(uint32_t PartID, uint32_t CurrentCluster);
		if(MyBPB->Bpb.NumDirEntries)
			DIREnts = (uint32_t) (MyBPB->Bpb.NumDirEntries);
		else
			DIREnts = (CountCluster(PartID, MyBPB->BpbExt.RootCluster) * ((uint32_t) ((uint32_t)(MyBPB->Bpb.BytesPerSector * MyBPB->Bpb.SectorsPerCluster) / (uint32_t)sizeof(FATDIR))));
		
		PFATDIR CurDirEnt = (PFATDIR) (CurrentPartition->FSdata->Buffer[2]);
		while (x < DIREnts) {
			if(CurDirEnt[x].Filename[0] && (CurDirEnt[x].Filename[0] != 0xE5))
				if(CompareFName((const char*)(CurDirEnt[x].Filename), (const char*)FName)) {
					memcpy((void*)MyDirEntry, (void*) &(CurDirEnt[x]), sizeof(FATDIR));
					return;
				}
			x++;
		}
		memset(MyDirEntry, 0, sizeof(FATDIR));
		return;
	} else {
		// We are in a sub directory...
		// PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
		uint32_t ClusterLength = (uint32_t) (MyBPB->Bpb.BytesPerSector * MyBPB->Bpb.SectorsPerCluster);

		PFATDIR DirBuffer = (PFATDIR) calloc(ClusterLength);
		ReadCluster(PartID, CurrentCluster, DirBuffer);
		
		while(CurrentCluster != 0xFFFFFFFF) {
			x = 0;
			while((x<((ClusterLength / 32)))) {
				if(DirBuffer[x].Filename[0] && (DirBuffer[x].Filename[0] != 0xE5))
					if(CompareFName((const char*)(DirBuffer[x].Filename), (const char*)FName)) {
						memcpy(MyDirEntry, (void*) &DirBuffer[x], sizeof(FATDIR));
						free(DirBuffer, ClusterLength);
						return;
					}
				x++;
			}
			CurrentCluster = NextCluster(PartID, CurrentCluster);
		}

		free(DirBuffer, ClusterLength);
		return;
	}
}

void CreateDir(uint32_t PartID, uint32_t ParrentCluster, char *DIRName)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	PFATDIR NewDirEnt = (PFATDIR) calloc(sizeof(FATDIR));
	PFATDIR NewDir = (PFATDIR) calloc((uint32_t)(MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector));

	uint32_t NewCluster = FindFreeCluster(PartID); // Find Free Cluster
	AllocateCluster(PartID, NewCluster); // Allocate Cluster
	sleep(2);
	//LinkCluster(PartID, NewCluster, 0xFFF);
// DirEntry
	// strcpy((uint8_t*)NewDirEnt->Filename, (const uint8_t*)DIRName);
	memcpy(NewDirEnt->Filename, DIRName, 11);
	NewDirEnt->Attrib = FAT_ATTRIB_DIR;
	if((CurrentPartition->FSid == FAT12FSid) || (CurrentPartition->FSid == FAT16FSid)) { // FAT 12/16
		NewDirEnt->FirstClusterHiBytes = 0;
		NewDirEnt->FirstCluster = (uint16_t) (NewCluster & 0xFFFF);
	} else { // FAT 32
		NewDirEnt->FirstClusterHiBytes = (uint16_t) (NewCluster >> 16);
		NewDirEnt->FirstCluster = (uint16_t) (NewCluster & 0xFFFF);
	}
// Dir
	memcpy(&NewDir[0], NewDirEnt, sizeof(FATDIR));
	strcpy((uint8_t*)NewDir[0].Filename, (const uint8_t*)".          ");
	strcpy((uint8_t*)NewDir[1].Filename, (const uint8_t*)"..         ");
	NewDir[1].FirstClusterHiBytes = (uint16_t) (ParrentCluster >> 16);
	NewDir[1].FirstCluster = (uint16_t) (ParrentCluster & 0xFFFF);
	NewDir[0].Attrib = FAT_ATTRIB_DIR;
	NewDir[1].Attrib = FAT_ATTRIB_DIR;

	WriteCluster(PartID, NewCluster, NewDir);
	WriteFAT(PartID);
	addDirEnt(PartID, NewDirEnt, ParrentCluster);
	free(NewDir, (uint32_t)(MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector));
	free(NewDirEnt, sizeof(FATDIR));
}

void addDirEnt(uint32_t PartID, PFATDIR DEnt, uint32_t ParrentCluster)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
	if(ParrentCluster) { // Not In Root DIR
		uint32_t DirEntriesPerCluster = (uint32_t)((MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector) / sizeof(FATDIR));
		void* ParrentBuffer = calloc((MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector));
		uint32_t CurrentCluster = ParrentCluster;
		uint32_t PrevCluster = CurrentCluster;
		while(CurrentCluster != 0xFFFFFFFF) {
			ReadCluster(PartID, CurrentCluster, ParrentBuffer);
			PFATDIR CurDir = (PFATDIR) ParrentBuffer;
			for(uint32_t e = 0; e < DirEntriesPerCluster; e++)
				if((!(CurDir[e].Filename[0])) || (CurDir[e].Filename[0] == 0xE5)) { // Do we have an empty entry?
					memcpy(&CurDir[e], DEnt, sizeof(FATDIR));
					WriteCluster(PartID, CurrentCluster, ParrentBuffer);
					free(ParrentBuffer, (MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector));
					return;
				}
			PrevCluster = CurrentCluster;
			CurrentCluster = NextCluster(PartID, CurrentCluster);
		}
		memset(ParrentBuffer, 0, ((MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector))); // Make sure buffer is clean!
		memcpy(ParrentBuffer, DEnt, sizeof(FATDIR)); // Place Entry Into Buffer
		uint32_t NewCluster = FindFreeCluster(PartID); // Find Free Cluster
		AllocateCluster(PartID, NewCluster); // Allocate Cluster
		LinkCluster(PartID, PrevCluster, NewCluster); // Link Cluster
		WriteCluster(PartID, NewCluster, ParrentBuffer); // write Cluster
		free(ParrentBuffer, (MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector)); // free buffer
		WriteFAT(PartID); // Update disk FAT
	} else { // Root DIR
		uint32_t DIREnts = 0;

		//uint32_t CountCluster(uint32_t PartID, uint32_t CurrentCluster);
		if(MyBPB->Bpb.NumDirEntries)
			DIREnts = (uint32_t) (MyBPB->Bpb.NumDirEntries);
		else
			DIREnts = (CountCluster(PartID, MyBPB->BpbExt.RootCluster) * ((uint32_t) ((uint32_t)(MyBPB->Bpb.BytesPerSector * MyBPB->Bpb.SectorsPerCluster) / (uint32_t)sizeof(FATDIR))));
		PFATDIR CurDir = (PFATDIR) CurrentPartition->FSdata->Buffer[2];
		for(uint32_t ent = 0; ent < DIREnts; ent++) {
			if((!(CurDir[ent].Filename[0])) || (CurDir[ent].Filename[0] == 0xE5)) { // Do we have an empty entry?
				memcpy(&(CurDir[ent]), DEnt, sizeof(FATDIR)); // Copy Entry into Root DIR Buffer
				WriteRootDir(PartID); // Update Root DIR on Disk
				return;
			}
		}
		// NOTE cluster bassed DIR is not currently extended when bounds are reached!
	}
}

void UpdateDirEntry(uint32_t PartID, PFATDIR DEnt, uint32_t ParrentCluster)
{
	PART_p CurrentPartition = getPartition(PartID);
	if(ParrentCluster) {
		// Sub Directory
		PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];
		uint32_t ClusterLength = (uint32_t) (MyBPB->Bpb.SectorsPerCluster * MyBPB->Bpb.BytesPerSector);
		uint32_t CurrentCluster = ParrentCluster;
		PFATDIR DirBuffer = (PFATDIR) calloc(ClusterLength);
		ReadCluster(PartID, CurrentCluster, DirBuffer);
		while(CurrentCluster != 0xFFFFFFFF) {
			int x = 0;
			while((x<((int)(ClusterLength / 32))) && (DirBuffer[x].Filename[0])) {
				if(CompareFName((const char*)(DirBuffer[x].Filename), (const char*)DEnt->Filename)) {
					memcpy((void*) &DirBuffer[x], DEnt, sizeof(FATDIR));
					WriteCluster(PartID, CurrentCluster, DirBuffer);
					free(DirBuffer, ClusterLength);
					return;
				}
				x++;
			}
			CurrentCluster = NextCluster(PartID, CurrentCluster);
		}

		free(DirBuffer, ClusterLength);
		return;
	} else {
		// Root Directory
		PFATDIR CurDirEnt = (PFATDIR) CurrentPartition->FSdata->Buffer[2];
		int x = 0;
		while (CurDirEnt[x].Filename[0]) {
			if(CompareFName((const char*)(CurDirEnt[x].Filename), (const char*)DEnt->Filename)) {
				memcpy((void*) &CurDirEnt[x], DEnt, sizeof(FATDIR));
				WriteRootDir(PartID);
				return;
			}
			x++;
		}
	}
}

void ReadRootDir(uint32_t PartID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];

	uint32_t RootStart = 0, RootLength = 0, RootCount = 0;

	//uint32_t CountCluster(uint32_t PartID, uint32_t CurrentCluster);
	if(MyBPB->Bpb.NumDirEntries)
		RootCount = (uint32_t) ((MyBPB->Bpb.NumDirEntries));// * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector);
	else
		RootCount = (CountCluster(PartID, MyBPB->BpbExt.RootCluster) * ((uint32_t) (MyBPB->Bpb.SectorsPerCluster) * (MyBPB->Bpb.BytesPerSector / sizeof(FATDIR))));
	RootLength = ((RootCount * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector);

	if(!CurrentPartition->FSdata->Buffer[2])
		CurrentPartition->FSdata->Buffer[2] = calloc((uint32_t)(RootLength * (uint32_t)MyBPB->Bpb.BytesPerSector));

	if(MyBPB->Bpb.NumDirEntries) {
		RootStart = (uint32_t) (((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors));
		PartRead(PartID, (void*) RootStart, CurrentPartition->FSdata->Buffer[2], RootLength);
	} else {
		RootStart = MyBPB->BpbExt.RootCluster;
		uint32_t x = 0;
		while(RootStart != 0xFFFFFFFF) {
			ReadCluster(PartID, RootStart, (void*)(((uint32_t)(CurrentPartition->FSdata->Buffer[2])) + (x * ((uint32_t) (MyBPB->Bpb.SectorsPerCluster)))));
			RootStart = NextCluster(PartID, RootStart);
			x++;
		}
	}
}

void WriteRootDir(uint32_t PartID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p CurrentPartition = getPartition(PartID);
	if(CurrentPartition->FSdata->Buffer[2]) {
		PFAT32BS MyBPB = (PFAT32BS) CurrentPartition->FSdata->Buffer[0];

		uint32_t RootStart = 0;

		if(MyBPB->Bpb.NumDirEntries) {
			RootStart = (uint32_t) (((MyBPB->Bpb.SectorsPerFat * MyBPB->Bpb.NumberOfFats) + MyBPB->Bpb.ReservedSectors));
			uint32_t RootLength = (uint32_t) ((MyBPB->Bpb.NumDirEntries * sizeof(FATDIR)) / MyBPB->Bpb.BytesPerSector);
			PartWrite(PartID, (void*) RootStart, CurrentPartition->FSdata->Buffer[2], RootLength);
		} else {
			RootStart = MyBPB->BpbExt.RootCluster;
			uint32_t x = 0;
			while(RootStart != 0xFFFFFFFF) {
				WriteCluster(PartID, RootStart, (void*)(((uint32_t)(CurrentPartition->FSdata->Buffer[2])) + (x * ((uint32_t) (MyBPB->Bpb.SectorsPerCluster)))));
				RootStart = NextCluster(PartID, RootStart);
				x++;
			}
		}
	}
}
