#include <hal.h>
#include <api/api.h>
#include <i386/memory/heapman.h>

#define MAX_HAL_ENTRIES 1024

typedef struct HAL_TABLE {
	HAL_p entry[MAX_HAL_ENTRIES];
} __attribute__((packed)) HALTable_t, *HALTable_P;

HALTable_P MyHALs;

void _HAL_init()
{
	MyHALs = (HALTable_P) calloc(sizeof(HALTable_t));
	_API_Add_Symbol("mkHAL", (uint32_t)&mkHAL);
	_API_Add_Symbol("rmHAL", (uint32_t)&rmHAL);
	_API_Add_Symbol("getHAL", (uint32_t)&getHAL);
	_API_Add_Symbol("installHAL", (uint32_t)&installHAL);
	_API_Add_Symbol("readHAL", (uint32_t)&readHAL);
	_API_Add_Symbol("writeHAL", (uint32_t)&writeHAL);
	_API_Add_Symbol("uninstallHAL", (uint32_t)&uninstallHAL);
	_API_Add_Symbol("getHALp", (uint32_t)&getHALp);
}

uint16_t mkHAL(HAL_p HALEnt)
{
	for(uint16_t h = 0; h < MAX_HAL_ENTRIES; h++)
		if(!MyHALs->entry[h]) {
			MyHALs->entry[h] = HALEnt;
			return h;
		}
	return 0xFFFF;
}

void rmHAL(uint16_t HALid)
{
	MyHALs->entry[HALid] = 0;
}

uint16_t getHAL(char* name)
{
	for(uint16_t x = 0; x < MAX_HAL_ENTRIES; x++)
		if(streql((const char*) MyHALs->entry[x]->name, (const char*) name))
			return x;
	return 0xFFFF;
}

HAL_p getHALp(uint16_t HALid)
{
	return MyHALs->entry[HALid];
}

void installHAL(uint16_t HALid, uint32_t PrivateID)
{
	if(MyHALs->entry[HALid] && MyHALs->entry[HALid]->install)
		MyHALs->entry[HALid]->install(PrivateID);
}

void readHAL(uint16_t HALid, uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
	if(MyHALs->entry[HALid] && MyHALs->entry[HALid]->read)
		MyHALs->entry[HALid]->read(PrivateID, Address, Buffer, Length);
}

void writeHAL(uint16_t HALid, uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
	if(MyHALs->entry[HALid] && MyHALs->entry[HALid]->write)
		MyHALs->entry[HALid]->write(PrivateID, Address, Buffer, Length);
}

void uninstallHAL(uint16_t HALid, uint32_t PrivateID)
{
	if(MyHALs->entry[HALid] && MyHALs->entry[HALid]->remove)
		MyHALs->entry[HALid]->remove(PrivateID);
}
