/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/IDE.C					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /*
NOTE:
	I'm very Lazy this time, this is PIO IDE ACCESS
	ALSO NO PCI SCAN hardwired to 1F0 and 170 BASES
	ALSO NO IRQ THIS IF FULL POLLING ONLY!
	NO ATAPI I/O (only HDD [ATA])
 */

#include "IDE.H"
#include "TIMER.H"
#include "../SYSTEM/DEVICE/DISKMAN.H"
#include <STDIO.H>

//#define DEBUG

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   Declare Local Functions				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _IDE_Write(uint16_t IDE, uint16_t OFF, uint8_t Data);
uint8_t _IDE_Read(uint16_t IDE, uint16_t OFF);
void _IDE_ReadBuffer(uint16_t IDE, void *BUFFER, int COUNT);
void _IDE_WriteBuffer(uint16_t IDE, void *BUFFER, int COUNT);
void _Probe_Disks(void);
error IDE_SectorIO(uint8_t drive, bool write, uint64_t start, uint16_t count, void* Buffer);


/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Variables					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _IDE_TIMEOUT 25//*~100mS = ~2500mS = ~2.5S
#define _IDE_RETRY 4

#define DMA_CHANNEL 2
#define IRQ_NUMBERa 14
#define IRQ_NUMBERb 15

#define IDE_ATA 0
#define IDE_ATAPI 1
#define ATA_MASTER 0
#define ATA_SLAVE 1


uint16_t _IDE_BASE[2] = { 0 };

enum _IDE_PORT_OFFSETS {
	DATA = 0,
	ERROR = 1,
	FEATURES = 1,
	SECCOUNT0 = 2,
	LBA0 = 3,
	LBA1 = 4,
	LBA2 = 5,
	HDDSEL = 6,
	COMMAND = 7,
	STATUS = 7,
	SECCOUNT1 = 8,
	LBA3 = 9,
	LBA4 = 0xA,
	LBA5 = 0xB,
	CTRL = 0xC,
	ALTSTATUS = 0xC,
	DEVADDR = 0xD
};

enum _STATUS_REGISTER_BREAKDOWN {
	_SR_ERR = 1,
	_SR_IDX = 2,
	_SR_CORR = 4,
	_SR_DRQ = 8,
	_SR_DSC = 0x10,
	_SR_DF = 0x20,
	_SR_DRDY = 0x40,
	_SR_BSY = 0x80
};

enum _IDE_COMMANDS {
	_CMD_READ_28 = 0x20,
	_CMD_WRITE_28 = 0x30,
	_CMD_READ_48 = 0x24,
	_CMD_WRITE_48 = 0x34,
	_CMD_FLUSH = 0xE7,
	_CMD_IDENTIFY = 0xEC,
	_CMD_PACKETID = 0xA1
};

struct _IDE_DEVICES_STRUCT {
	bool Exists;      // 0 (Empty) or 1 (This Drive really exists).
	uint8_t Channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
	uint8_t Drive;       // 0 (Master Drive) or 1 (Slave Drive).
	unsigned short Type;        // 0: ATA, 1:ATAPI.
	unsigned short Signature;   // Drive Signature
	unsigned short Capabilities;// Features.
	unsigned int CommandSets; // Command Sets Supported.
	unsigned int Secotrs;     // Size in Sectors.
	uint8_t Model[41];   // Model in string.
} _IDE_DRIVE[4];

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _IDE_Write(uint16_t IDE, uint16_t OFF, uint8_t Data)
{
#ifdef DEBUG
	txf(1, "(IDE.C:Line 106) _IDE_Write(0x%x, 0x%x, 0x%x)\n\r", IDE, OFF, Data);
#endif
	outb((uint16_t)(IDE+OFF), Data);
}

uint8_t _IDE_Read(uint16_t IDE, uint16_t OFF)
{
#ifdef DEBUG
	txf(1, "(IDE.C:Line 114) _IDE_Read(0x%x, 0x%x) = ", IDE, OFF);
	uint8_t Data = inb((uint16_t)(IDE+(uint16_t)OFF));
	txf(1, "0x%x\n\r", Data);
	return Data;
#endif
	return inb((uint16_t)(IDE+OFF));
}

void _IDE_ReadBuffer(uint16_t IDE, void *BUFFER, int COUNT) //512Byte * COUNT read to buffer FROM DATA REG
{
	uint16_t *buffer = (uint16_t *) BUFFER;
	for (int c = 0; c < COUNT; c++) {
		for(int t = 0; t < 256; t++)
			buffer[(c*256)+t] = inw(IDE+DATA);
		timer_wait(2);
	}
}

void _IDE_WriteBuffer(uint16_t IDE, void *BUFFER, int COUNT) //512Byte * COUNT read to buffer FROM DATA REG
{
	uint16_t *buffer = (uint16_t *) BUFFER;
	for (int c = 0; c < COUNT; c++) {
		for(int t = 0; t < 256; t++)
			outw(IDE+DATA, buffer[(c*256)+t]);
		timer_wait(2);
	}
}

void _Probe_Disks()
{
	uint8_t count = 0;
	for(uint8_t channel = 0; channel <= 1; channel++)
		for(uint8_t drive = 0; drive <= 1; drive++) {
			uint8_t err=0, type=IDE_ATA, status;
			_IDE_DRIVE[count].Exists = false;
			_IDE_Write(_IDE_BASE[channel], HDDSEL, (0xA0 | (uint8_t) (drive << 4)));	//Select Drive
			timer_wait(1);	// wait ~100mS
			_IDE_Write(_IDE_BASE[channel], COMMAND, _CMD_IDENTIFY);
			timer_wait(1);
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
				timer_wait(1);	// wait ~100mS
				_IDE_Write(_IDE_BASE[channel], COMMAND, _CMD_PACKETID);
				timer_wait(1);
				status = _IDE_Read(_IDE_BASE[channel], STATUS);
				if(!status || status & _SR_ERR)
					continue;
				type = IDE_ATAPI;
			}
			uint8_t Buffer[512] = { 0 };
			_IDE_ReadBuffer(_IDE_BASE[channel], (uint32_t *) &Buffer, 1);
			_IDE_DRIVE[count].Exists = true;
			_IDE_DRIVE[count].Type = type;
			_IDE_DRIVE[count].Channel = channel;
			_IDE_DRIVE[count].Drive = drive;
			_IDE_DRIVE[count].Signature = *((unsigned short*)&Buffer[0]);
			_IDE_DRIVE[count].Capabilities = *((unsigned short*)&Buffer[98]);
			_IDE_DRIVE[count].CommandSets = *((unsigned int*)&Buffer[164]);
			if (_IDE_DRIVE[count].CommandSets & (1 << 26))
				_IDE_DRIVE[count].Secotrs = *((unsigned int*)&Buffer[200]);
			else
				_IDE_DRIVE[count].Secotrs = *((unsigned int*)&Buffer[120]);

			for(int k = 0; k < 40; k += 2) {
				_IDE_DRIVE[count].Model[k] = Buffer[54 + k + 1];
				_IDE_DRIVE[count].Model[k + 1] = Buffer[54 + k];
			}
			_IDE_DRIVE[count].Model[40] = '\0';

			count++;
		}
}

error IDE_SectorIO(uint8_t drive, bool write, uint64_t start, uint16_t count, void* Buffer)
{
	bool LBA48 = false;
	if(start+(uint64_t)count > 0xFFFFFFF || count > 0xFF)
		LBA48 = true;
	if (!_IDE_DRIVE[drive].Exists || start+(uint64_t)count > _IDE_DRIVE[drive].Secotrs || !count)
		return ERROR_INPUT;
	_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], HDDSEL, (0xE0 | (uint8_t) (_IDE_DRIVE[drive].Drive << 4 | (int)((start >> 24) & 0xF))) );	//Select Drive
	timer_wait(1);
	_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], FEATURES, 0);
	if(LBA48) {
		_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], SECCOUNT0, (uint8_t) ((count >> 8) & 0xFF));
		_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], LBA0, (uint8_t) ((start >> 24) & 0xFF));
		_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], LBA1, (uint8_t) ((start >> 32) & 0xFF));
		_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], LBA2, (uint8_t) ((start >> 40) & 0xFF));
	}
	_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], SECCOUNT0, (uint8_t) (count & 0xFF));
	_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], LBA0, (uint8_t) (start & 0xFF));
	_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], LBA1, (uint8_t) ((start >> 8) & 0xFF));
	_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], LBA2, (uint8_t) ((start >> 16) & 0xFF));
	_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], COMMAND,
		((LBA48) ? ((write) ? _CMD_WRITE_48 : _CMD_READ_48) : ((write) ? _CMD_WRITE_28 : _CMD_READ_28)));
	int to = 0;
	while(!(_IDE_Read(_IDE_BASE[_IDE_DRIVE[drive].Channel], STATUS) & _SR_DRQ)) {
		timer_wait(1);
		to++;
		if(to >= _IDE_TIMEOUT)
			return ERROR_TIMEOUT;
	}
	if(!write)
		_IDE_ReadBuffer(_IDE_BASE[_IDE_DRIVE[drive].Channel], (uint32_t *) Buffer, count);
	else {
		_IDE_WriteBuffer(_IDE_BASE[_IDE_DRIVE[drive].Channel], (uint32_t *) Buffer, count);
		_IDE_Write(_IDE_BASE[_IDE_DRIVE[drive].Channel], COMMAND, _CMD_FLUSH);
	}
	return ERROR_NONE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	  Public Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _IDE_init()
{
#ifdef DEBUG
	txf(1, "_IDE_init()\n\r");
#endif
	//I'm Lazy today I'll Search through PCI in next build, just want this working
	_IDE_BASE[0] = 0x1F0;
	_IDE_BASE[1] = 0x170;

	_Probe_Disks();

	for(int x = 0; x < 4; x ++)
		if(_IDE_DRIVE[x].Exists)
			Install_Disk((uint8_t)x, _IDE_DRIVE[x].Secotrs, &_IDE_IN, &_IDE_OUT);	//Install Disks in Disk Manager.
#ifdef DEBUG
	txf(1, "Found Disks:\n\r");
	for(int x = 0; x < 4; x ++)
		if(_IDE_DRIVE[x].Exists)
			txf(1, "%s %s %i MB %s \"%s\"\n\r", ((_IDE_DRIVE[x].Channel) ? "SECONDARY" : "PRIMARY"), ((_IDE_DRIVE[x].Drive) ? "SLAVE" : "MASTER"),
				(_IDE_DRIVE[x].Secotrs / 2048), ((_IDE_DRIVE[x].Type) ? "ATAPI" : "ATA"), _IDE_DRIVE[x].Model);
#endif
}

error _IDE_IO(uint8_t drive, bool write, uint64_t start, uint16_t count, void* Buffer)
{
	error ERRORret = ERROR_NONE;
	for (int t = 0; t < _IDE_RETRY; t++)
		if(!(ERRORret =  IDE_SectorIO(drive, write, start, count, Buffer)))
			break;
	

	return ERRORret;
}

void _IDE_IN(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count)
{
	*ERRORCODE = _IDE_IO(LOCALID, false, Start, count, Buffer);
}

void _IDE_OUT(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count)
{
	*ERRORCODE = _IDE_IO(LOCALID, true, Start, count, Buffer);
}