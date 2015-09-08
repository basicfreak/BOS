#include <hal.h>
#include "pci.h"
#include "pciget.h"

_PCI_TBL_p PCI_Table;
HAL_p MyHAL;
uint32_t PCI_Count;

#define DEBUG
#define DEBUG_FULL

uint32_t PCI_Find_ByClass(uint8_t Class, uint8_t SubClass, uint8_t ProgIF);
uint32_t PCI_Find_ByDevice(uint16_t Vendor, uint16_t Device);
void read(uint32_t PrivateID, void *Address, void *Buffer, uint32_t Length);

int init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	PCI_Count = 0;
	PCI_Table = (_PCI_TBL_p) calloc(sizeof(_PCI_TBL_t));
	for (uint8_t bus = 0; bus < 0xFF; bus++)
		for (uint8_t slot = 0; slot < 0x20; slot++) {
			for (uint8_t function = 0; function < 8; function ++) {
				if ((_PCI_CONFIG_getWord(bus, slot, function, 0) != 0xFFFF) && (_PCI_CONFIG_getWord(bus, slot, function, 0) != 0)) {        // Does device exist?
					PCI_Table->Device[PCI_Count] = (_PCI_DEV_p) calloc(sizeof(_PCI_DEV_t));

					PCI_Table->Device[PCI_Count]->BUS = bus;
					PCI_Table->Device[PCI_Count]->SLOT = slot;
					PCI_Table->Device[PCI_Count]->FUNCTION = function;
					PCI_Table->Device[PCI_Count]->Vendor = _PCI_CONFIG_getWord(bus, slot, function, 0);
					PCI_Table->Device[PCI_Count]->Device = _PCI_CONFIG_getWord(bus, slot, function, 2);
					PCI_Table->Device[PCI_Count]->Class = _PCI_CONFIG_getByte(bus, slot, function, 11);
					PCI_Table->Device[PCI_Count]->SubClass = _PCI_CONFIG_getByte(bus, slot, function, 10);
					PCI_Table->Device[PCI_Count]->ProgIF = _PCI_CONFIG_getByte(bus, slot, function, 9);
					PCI_Table->Device[PCI_Count]->HeaderType = _PCI_CONFIG_getByte(bus, slot, function, 14);
					PCI_Table->Device[PCI_Count]->BAR0 = _PCI_CONFIG_getLong(bus, slot, function, 16);
					PCI_Table->Device[PCI_Count]->INTLine = _PCI_CONFIG_getByte(bus, slot, function, 0x3C);
					PCI_Count++;
				}
			}
		}
#ifdef DEBUG_FULL
	DEBUG_printf("PCI Table:\n");
	for(uint32_t x = 0; x < PCI_Count; x ++)
		DEBUG_printf("%x:%x:%x %x:%x %x:%x:%x %x %x (%i)\n", PCI_Table->Device[x]->BUS, PCI_Table->Device[x]->SLOT, PCI_Table->Device[x]->FUNCTION,
				PCI_Table->Device[x]->Vendor, PCI_Table->Device[x]->Device, PCI_Table->Device[x]->Class, PCI_Table->Device[x]->SubClass,
				PCI_Table->Device[x]->ProgIF, PCI_Table->Device[x]->BAR0, PCI_Table->Device[x]->INTLine, PCI_Table->Device[x]->HeaderType);
#endif
	MyHAL = (HAL_p) calloc(sizeof(HAL_t));
	MyHAL->install = 0;
	MyHAL->read = read;
	MyHAL->write = 0;
	MyHAL->remove = 0;
	MyHAL->flags = (HAL_F_PRESENT | HAL_F_READ | HAL_F_HARDWARE);
	strcpy(MyHAL->name, "PCI");
	uint16_t MyHALid = mkHAL(MyHAL);
#ifdef DEBUG_FULL
	DEBUG_printf("PCI HAL ID = %x (%i)\n", MyHALid, MyHALid);
	DEBUG_printf("Test Find By Class 6:1:0 = 0x%x\n", PCI_Find_ByClass(6, 1, 0));
	DEBUG_printf("Test Find By Device 8086:7010 = 0x%x\n", PCI_Find_ByDevice(0x8086, 0x7010));
	uint32_t HTest0 = 0, HTest1 = 0;

	readHAL(MyHALid, 0, (void*) (0xFFFF0601), (void*) &HTest0, 0x00000000);
	readHAL(MyHALid, 0, (void*) (0x80867010), (void*) &HTest1, 0x00000000);

	DEBUG_printf("Same Results By HAL? 0x%x, 0x%x\n", HTest0, HTest1);

	DEBUG_printf("Dump Of 1234:1111:\n");
	readHAL(MyHALid, 0, (void*) 0x12341111, (void*) &HTest0, 0);
	void* MyBuf0 = calloc(256);
	readHAL(MyHALid, HTest0, 0, MyBuf0, 256);
	for(int x = 0; x < 256; x ++)
		DEBUG_printf("%x ", ((uint8_t*)MyBuf0)[x]);
	free(MyBuf0, 256);
	DEBUG_printf("\nNow Just BAR0:\n");
	MyBuf0 = calloc(4);
	readHAL(MyHALid, HTest0, 16, MyBuf0, 4);
	for(int x = 0; x < 4; x ++)
		DEBUG_printf("%x ", ((uint8_t*)MyBuf0)[x]);
	free(MyBuf0, 4);
	DEBUG_printf("\n");
#endif

	return 0;
}

/*typedef struct _PCI_DEVICES { // 16 Bytes
		uint8_t BUS, SLOT, FUNCTION;
		uint16_t Vendor, Device;
		uint8_t Class, SubClass, ProgIF, HeaderType;
		uint8_t INTLine;
		uint32_t BAR0;
	} _PCI_DEV_t, *_PCI_DEV_p;*/

uint32_t PCI_Find_ByClass(uint8_t Class, uint8_t SubClass, uint8_t ProgIF)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
	DEBUG_printf("\t0x%x, 0x%x, 0x%x\n", Class, SubClass, ProgIF);
#endif
	uint32_t ret = 0;
	for(uint32_t pcidev = 0; pcidev < PCI_Count; pcidev++)
		if((PCI_Table->Device[pcidev]->Class == Class) && (PCI_Table->Device[pcidev]->SubClass == SubClass))
			if((!ProgIF) || (PCI_Table->Device[pcidev]->ProgIF == ProgIF))
				ret = (uint32_t) ((PCI_Table->Device[pcidev]->BUS << 16) | (PCI_Table->Device[pcidev]->SLOT << 8) | PCI_Table->Device[pcidev]->FUNCTION);
	return ret;
}

uint32_t PCI_Find_ByDevice(uint16_t Vendor, uint16_t Device)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
	DEBUG_printf("\t0x%x, 0x%x\n", Vendor, Device);
#endif
	uint32_t ret = 0;
	for(uint32_t pcidev = 0; pcidev < PCI_Count; pcidev++)
		if((PCI_Table->Device[pcidev]->Vendor == Vendor) && (PCI_Table->Device[pcidev]->Device == Device))
			ret = (uint32_t) ((PCI_Table->Device[pcidev]->BUS << 16) | (PCI_Table->Device[pcidev]->SLOT << 8) | PCI_Table->Device[pcidev]->FUNCTION);
	return ret;
}

void read(uint32_t PrivateID, void *Address, void *Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
	DEBUG_printf("\t0x%x, 0x%x, 0x%x, 0x%x\n", PrivateID, (uint32_t) Address, (uint32_t) Buffer, Length);
#endif
	//Private ID = (BUS<<16) | (SLOT<<8) | FUNCT OR 0 for a search
	uint32_t Addr = (uint32_t) Address;
	if(!PrivateID) {
		// We are searching
		// Address is uint32_t if | 0xFFFF0000 we are looking for (Class << 8) | Subclass and Length & 0xFF is ProgIF
		// Else Address is (VENDORID << 16) | DEVICEID
		// Buffer return is uint32_t of Resulting Private ID
		uint32_t *Pid = (uint32_t*)Buffer;
		if((Addr & 0xFFFF0000) == 0xFFFF0000)
			Pid[0] = PCI_Find_ByClass((uint8_t) ((Addr >> 8) & 0xFF), (uint8_t) (Addr & 0xFF), (uint8_t) (Length & 0xFF));
		else
			Pid[0] = PCI_Find_ByDevice((uint16_t) ((Addr >> 16) & 0xFFFF), (uint16_t) (Addr & 0xFFFF));
		DEBUG_printf("\t\tPid = 0x%x\n", Pid);
	} else {
		// We are reading
		// Address is uint32_t, if 0 && Length 256 we are getting full read
		// else partial read
		// Buffer is output.
		if((!Address) && Length == 256)
			_PCI_CONFIG_Dump((uint8_t) ((PrivateID >> 16) & 0xFF), (uint8_t) ((PrivateID >> 8) & 0xFF), (uint8_t) (PrivateID & 0xFF), Buffer);
		else {
			uint8_t *Buf = (uint8_t*) Buffer;
			for(uint32_t x = 0; x < Length; x++)
				Buf[x] = _PCI_CONFIG_getByte((uint8_t) ((PrivateID >> 16) & 0xFF), (uint8_t) ((PrivateID >> 8) & 0xFF), (uint8_t) (PrivateID & 0xFF), (Addr + x));
		}

	}
}
