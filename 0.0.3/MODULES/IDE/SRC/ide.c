#include <hal.h>
#include <pci.h>
#include "ide.h"

#undef DEBUG_FULL

void read(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
void write(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);

HAL_p MyHAL;
uint16_t PCI_HAL = 0xFFFF;
uint16_t MBR_HAL = 0xFFFF;
uint16_t MyHALid = 0xFFFF;
uint32_t PCI_ADDR = 0;

PCI_H_00 *IDE_PCI_HEAD;
_IDE_DRIVE *_MyDrives;
int init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif

	MBR_HAL = getHAL("MBR");
	if(MBR_HAL == 0xFFFF)
		return -1;

	PCI_HAL = getHAL("PCI");
	if(PCI_HAL == 0xFFFF)
		return -1;

	readHAL(PCI_HAL, 0, (void*) (0xFFFF0101), (void*) &PCI_ADDR, 0x00000000);
	if(!PCI_ADDR)
		return -1;

	IDE_PCI_HEAD = (PCI_H_00*) calloc(sizeof(PCI_H_00));

	readHAL(PCI_HAL, PCI_ADDR, 0, (void*) IDE_PCI_HEAD, 256);

	_MyDrives = (_IDE_DRIVE*) calloc(sizeof(_IDE_DRIVE) * 4);
	_IDE_BASE = (uint16_t*) calloc(4);
	
	_IDE_BASE[0] = 0x1F0;
	_IDE_BASE[1] = 0x170;

	_Probe_Disks();
#ifndef DEBUG
	DEBUG_printf("Found Disks:\n");
	for(int x = 0; x < 4; x ++)
		if(_MyDrives[x].Exists)
			DEBUG_printf("%s %s %i MB\t%s\n\t%s\n", ((_MyDrives[x].Channel) ? ((const char*)"SECONDARY") : ((const char*)"PRIMARY")), ((_MyDrives[x].Drive) ? ((const char*)"SLAVE") : ((const char*)"MASTER")),
				(_MyDrives[x].Secotrs / 2048), ((_MyDrives[x].Type) ? ((const char*)"ATAPI") : ((const char*)"ATA")), _MyDrives[x].Model);
#endif

	MyHAL = (HAL_p) calloc(sizeof(HAL_t));

	MyHAL->install = 0;
	MyHAL->read = read;
	MyHAL->write = write;
	MyHAL->remove = 0;
	MyHAL->flags = (HAL_F_PRESENT | HAL_F_READ | HAL_F_WRITE | HAL_F_DISK | HAL_F_MBR);
	strcpy(MyHAL->name, "IDE");
	MyHALid = mkHAL(MyHAL);
	for(int x = 0; x < 4; x ++)
			if(_MyDrives[x].Exists)
				installHAL(MBR_HAL, (uint32_t)(x << 16 | MyHALid));


	/*void* Buffer = calloc(4096);
	(void) IDE_SectorIO(0, FALSE, 0, 1, Buffer);
	uint8_t *MyDat = (uint8_t*) Buffer;
	DEBUG_printf("IDE 0, Sector 0:\n");
	for(int x=0; x<512; x++)
		DEBUG_printf("%x ", MyDat[x]);
	DEBUG_printf("\n");


	free(Buffer, 4096);*/


#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	return 0;
}

void read(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	IDE_SectorIO((uint8_t) PrivateID, FALSE, (uint64_t) Address, (uint8_t) Length, Buffer);
}

void write(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	IDE_SectorIO((uint8_t) PrivateID, TRUE, (uint64_t) Address, (uint8_t) Length, Buffer);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _IDE_Write(uint16_t IDE, uint16_t OFF, uint8_t Data)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	outb((uint16_t)(IDE+(uint16_t)OFF), Data);
}

uint8_t _IDE_Read(uint16_t IDE, uint16_t OFF)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	return inb((uint16_t)(IDE+(uint16_t)OFF));
}

void _IDE_ReadBuffer(uint16_t IDE, void *BUFFER, int COUNT) //512Byte * COUNT read to buffer FROM DATA REG
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint16_t *buffer = (uint16_t *) BUFFER;
	for (int c = 0; c < COUNT; c++) {
		for(int t = 0; t < 256; t++)
			buffer[(c*256)+t] = inw(IDE+DATA);
		PIT_wait(5);
	}
}

void _IDE_WriteBuffer(uint16_t IDE, void *BUFFER, int COUNT) //512Byte * COUNT read to buffer FROM DATA REG
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint16_t *buffer = (uint16_t *) BUFFER;
	for (int c = 0; c < COUNT; c++) {
		for(int t = 0; t < 256; t++)
			outw(IDE+DATA, buffer[(c*256)+t]);
		PIT_wait(5);
	}
}

void _Probe_Disks()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint8_t count = 0;
	for(uint8_t channel = 0; channel <= 1; channel++)
		for(uint8_t drive = 0; drive <= 1; drive++) {
			uint8_t err=0, type=IDE_ATA, status;
			_MyDrives[count].Exists = false;
			_IDE_Write(_IDE_BASE[channel], HDDSEL, (0xA0 | (uint8_t) (drive << 4)));	//Select Drive
			PIT_wait(5);	// wait ~100mS
			_IDE_Write(_IDE_BASE[channel], COMMAND, _CMD_IDENTIFY);
			PIT_wait(5);
			status = _IDE_Read(_IDE_BASE[channel], STATUS);
			if(!status || status == _SR_ERR) // This device does not exist
				continue; // we are done here lets go to next device
			while(true) {
				status = _IDE_Read(_IDE_BASE[channel], STATUS);
				if(status & _SR_ERR) {
					err = 1;
					break;
				}
				if(!(status & _SR_BSY) && (status & _SR_DRQ))
					break;
			}
			if(err) {
				_IDE_Write(_IDE_BASE[channel], HDDSEL, (0xA0 | (uint8_t) (drive << 4)));	//Select Drive
				PIT_wait(5);	// wait ~100mS
				_IDE_Write(_IDE_BASE[channel], COMMAND, _CMD_PACKETID);
				PIT_wait(5);
				status = _IDE_Read(_IDE_BASE[channel], STATUS);
				if(!status || status & _SR_ERR)
					continue;
				type = IDE_ATAPI;
			}
			uint8_t *Buffer = (uint8_t*) calloc(512);
			_IDE_ReadBuffer(_IDE_BASE[channel], (uint32_t *) Buffer, 1);
			_MyDrives[count].Exists = true;
			_MyDrives[count].Type = type;
			_MyDrives[count].Channel = channel;
			_MyDrives[count].Drive = drive;
			_MyDrives[count].Signature = ((unsigned short*)&Buffer[0])[0];
			_MyDrives[count].Capabilities = ((unsigned short*)&Buffer[98])[0];
			_MyDrives[count].CommandSets = ((unsigned int*)&Buffer[164])[0];
			if (_MyDrives[count].CommandSets & (1 << 26))
				_MyDrives[count].Secotrs = ((unsigned int*)&Buffer[200])[0];
			else
				_MyDrives[count].Secotrs = ((unsigned int*)&Buffer[120])[0];

			for(int k = 0; k < 40; k += 2) {
				_MyDrives[count].Model[k] = Buffer[54 + k + 1];
				_MyDrives[count].Model[k + 1] = Buffer[54 + k];
			}
			_MyDrives[count].Model[40] = '\0';

			count++;
		}
}

bool IDE_SectorIO(uint8_t drive, bool write, uint64_t start, uint8_t count, void* Buffer)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	bool LBA48 = false;
	if(start+(uint64_t)count > 0xFFFFFFF)
		LBA48 = true;
	DEBUG_printf("%s", ((LBA48) ? "-LBA48-" : "-LBA28-"));
	if (!_MyDrives[drive].Exists || start+(uint64_t)count > _MyDrives[drive].Secotrs || !count)
		return FALSE;
	_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], HDDSEL, (0xE0 | (uint8_t) (_MyDrives[drive].Drive << 4 | (int)((start >> 24) & 0xF))) );	//Select Drive
	PIT_wait(5);
	_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], FEATURES, 0);
	if(LBA48) {
		_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], SECCOUNT0, 0);
		_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], LBA0, (uint8_t) ((start >> 24) & 0xFF));
		_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], LBA1, (uint8_t) ((start >> 32) & 0xFF));
		_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], LBA2, (uint8_t) ((start >> 40) & 0xFF));
	}
	_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], SECCOUNT0, count);
	_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], LBA0, (uint8_t) (start & 0xFF));
	_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], LBA1, (uint8_t) ((start >> 8) & 0xFF));
	_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], LBA2, (uint8_t) ((start >> 16) & 0xFF));
	_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], COMMAND,
		((LBA48) ? ((write) ? _CMD_WRITE_48 : _CMD_READ_48) : ((write) ? _CMD_WRITE_28 : _CMD_READ_28)));
	int to = 0;
	while(!(_IDE_Read(_IDE_BASE[_MyDrives[drive].Channel], STATUS) & _SR_DRQ)) {
		PIT_wait(5);
		to++;
		if(to >= _IDE_TIMEOUT)
			return FALSE;
	}
	DEBUG_printf("\n0x%x\n", _IDE_Read(_IDE_BASE[_MyDrives[drive].Channel], STATUS));
	if(!write)
		_IDE_ReadBuffer(_IDE_BASE[_MyDrives[drive].Channel], (uint32_t *) Buffer, count);
	else {
		_IDE_WriteBuffer(_IDE_BASE[_MyDrives[drive].Channel], (uint32_t *) Buffer, count);
		_IDE_Write(_IDE_BASE[_MyDrives[drive].Channel], COMMAND, _CMD_FLUSH);
	}

#ifdef DEBUG_FULL
	uint8_t* TBuff = (uint8_t*) Buffer;
	for(uint32_t x = 0; x < (count * 0x200); x++)
		DEBUG_printf("%x ", TBuff[x]);
	DEBUG_printf("\n");
#endif

	return TRUE;
}
