#include <vfs.h>
#include <hal.h>
#include <i386/memory/heapman.h>
#include <api/api.h>

#define MAX_PARTITIONS 1024
#define MAX_FILESYSTEMS 256

typedef struct PartitionTable {
	PART_p entry[MAX_PARTITIONS];
}__attribute__((packed)) PartT_t, *PartT_p;

typedef struct FSTable {
	FS_p entry[MAX_FILESYSTEMS];
}__attribute__((packed)) FST_t, *FST_p;

PartT_p MyPartitions;
FST_p MyFileSystems;

void _VFS_init()
{
	MyPartitions = (PartT_p) calloc(sizeof(PartT_t));
	MyFileSystems = (FST_p) calloc(sizeof(FST_t));
	_API_Add_Symbol("fdir", (uint32_t) &fdir);
	_API_Add_Symbol("fnew", (uint32_t) &fnew);
	_API_Add_Symbol("fnewDIR", (uint32_t) &fnewDIR);
	_API_Add_Symbol("fopen", (uint32_t) &fopen);
	_API_Add_Symbol("fclose", (uint32_t) &fclose);
	_API_Add_Symbol("fread", (uint32_t) &fread);
	_API_Add_Symbol("fwrite", (uint32_t) &fwrite);
	_API_Add_Symbol("fmount", (uint32_t) &fmount);
	_API_Add_Symbol("mkPartition", (uint32_t) &mkPartition);
	_API_Add_Symbol("rmPartition", (uint32_t) &rmPartition);
	_API_Add_Symbol("mkFileSystem", (uint32_t) &mkFileSystem);
	_API_Add_Symbol("rmFileSystem", (uint32_t) &rmFileSystem);
	_API_Add_Symbol("PartRead", (uint32_t) &PartRead);
	_API_Add_Symbol("PartWrite", (uint32_t) &PartWrite);
	_API_Add_Symbol("getPartition", (uint32_t) &getPartition);
}


uint32_t GetPartByName(char* name);
uint32_t GetPartByName(char* name)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	for (uint32_t x = 0; x < MAX_PARTITIONS; x++)
		if(streql((const uint8_t*)MyPartitions->entry[x]->Name, (const uint8_t*)name))
			return x;
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	return 0xFFFFFFFF;
}

FILE *fdir(char* path)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	FILE *Dir = fopen(path);
	if((Dir->Flags != FF_INVALID) && (Dir->Flags & FF_DIR)) {
		if(MyFileSystems->entry[MyPartitions->entry[Dir->PartID]->FSid]->directory)
			MyFileSystems->entry[MyPartitions->entry[Dir->PartID]->FSid]->directory(Dir);
	}
	return Dir;
}

FILE *fnew(char* path)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	FILE *RetFile = (FILE*) calloc(sizeof(FILE));
	char **MyPath = strexp(path, '/');
	int x = 0;
	while(MyPath[x++]);
	x--;
	x--;

	char* tempCharPointer = MyPath[x];
	strcpy(RetFile->Name, MyPath[x]);
	// free(MyPath[x], 128);
	MyPath[x] = (char*)0;

	RetFile->PartID = GetPartByName(MyPath[1]);
	if(MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid])
		if(MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid]->newFile)
			MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid]->newFile(RetFile, MyPath);

	MyPath[x] = tempCharPointer;
	freestrexp(MyPath);

	return RetFile;
}


void fnewDIR(char* path)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	FILE *RetFile = (FILE*) calloc(sizeof(FILE));
	char **MyPath = strexp(path, '/');
	int x = 0;
	while(MyPath[x++]);
	x--;
x--;
	strcpy(RetFile->Name, MyPath[x]);
DEBUG_printf("A");
	// free(MyPath[x], 128);
	char* tempCharPointer = MyPath[x];
DEBUG_printf("B");
	MyPath[x] = (char*)0;
DEBUG_printf("C");
	RetFile->Flags = FF_DIR;

	RetFile->PartID = GetPartByName(MyPath[1]);
	if(MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid])
		if(MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid]->newFile)
			MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid]->newFile(RetFile, MyPath);

DEBUG_printf("D");
	MyPath[x] = tempCharPointer;
	freestrexp(MyPath);

DEBUG_printf("E");
	fclose(RetFile);

DEBUG_printf("F");
}

FILE *fopen(char* path)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	FILE *RetFile = (FILE*) calloc(sizeof(FILE));
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	RetFile->Flags = FF_INVALID;

	// path should be in this format /PARTITION/DIR/DIR/FILE
	char **MyPath = strexp(path, '/');
	RetFile->PartID = GetPartByName(MyPath[1]);
#ifdef DEBUG
	DEBUG_printf("%i\n", RetFile->PartID);
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(RetFile->PartID != 0xFFFFFFFF)
		if(MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid])
			if(MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid]->open)
				MyFileSystems->entry[MyPartitions->entry[RetFile->PartID]->FSid]->open(RetFile, MyPath);
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	freestrexp(MyPath);
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	return RetFile;
}

void fclose(FILE *file)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	free(file, sizeof(FILE));
}

void fread(FILE *file, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(file->Flags != FF_INVALID) {
		uint32_t FSid = MyPartitions->entry[file->PartID]->FSid;
		if(MyFileSystems->entry[FSid])
			if(MyFileSystems->entry[FSid]->read)
				MyFileSystems->entry[FSid]->read(file, Address, Buffer, Length);
	}
}


void fwrite(FILE *file, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(file->Flags != FF_INVALID) {
		uint32_t FSid = MyPartitions->entry[file->PartID]->FSid;
		if(MyFileSystems->entry[FSid])
			if(MyFileSystems->entry[FSid]->write)
				MyFileSystems->entry[FSid]->write(file, Address, Buffer, Length);
	}
}

void fmount(uint32_t PartID)
{
	if(MyFileSystems->entry[MyPartitions->entry[PartID]->FSid])
		if(MyFileSystems->entry[MyPartitions->entry[PartID]->FSid]->mount)
			MyFileSystems->entry[MyPartitions->entry[PartID]->FSid]->mount(PartID);
}

void fumount(uint32_t PartID)
{
	if(MyFileSystems->entry[MyPartitions->entry[PartID]->FSid])
		if(MyFileSystems->entry[MyPartitions->entry[PartID]->FSid]->umount)
			MyFileSystems->entry[MyPartitions->entry[PartID]->FSid]->umount(PartID);
}

uint32_t mkPartition(PART_p PartPointer)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	for(uint32_t p = 0; p < MAX_PARTITIONS; p++)
		if(!MyPartitions->entry[p]) {
			MyPartitions->entry[p] = PartPointer;
			return p;
		}
	return 0xFFFFFFFF;
}


void rmPartition(uint32_t PartID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	MyPartitions->entry[PartID] = 0;
}


void mkFileSystem(uint16_t FSid, FS_p FSPointer)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	MyFileSystems->entry[FSid] = FSPointer;
}


void rmFileSystem(uint32_t FSid)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	MyFileSystems->entry[FSid] = 0;
}


void PartRead(uint32_t PartID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p tempPartition = MyPartitions->entry[PartID];
	if(((uint32_t) Address + Length) <= tempPartition->Length)
		readHAL(tempPartition->HALid, tempPartition->PrivateID, (void*)(tempPartition->Start + (uint32_t) Address), Buffer, Length);
}


void PartWrite(uint32_t PartID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PART_p tempPartition = MyPartitions->entry[PartID];
	if(((uint32_t) Address + Length) <= tempPartition->Length)
		writeHAL(tempPartition->HALid, tempPartition->PrivateID, (void*)(tempPartition->Start + (uint32_t) Address), Buffer, Length);
}

PART_p getPartition(uint32_t PartID)
{
#ifdef DEBUG
	// DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	return MyPartitions->entry[PartID];
}
