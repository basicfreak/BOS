#ifndef _IDE_H_INCLUDED
	#define _IDE_H_INCLUDED

	#include <api.h>

	/* * * * * * * * * * * * * * * * * * * * * * * * * * *
	 *			   Declare Local Functions				 *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void _IDE_Write(uint16_t IDE, uint16_t OFF, uint8_t Data);
	uint8_t _IDE_Read(uint16_t IDE, uint16_t OFF);
	void _IDE_ReadBuffer(uint16_t IDE, void *BUFFER, int COUNT);
	void _IDE_WriteBuffer(uint16_t IDE, void *BUFFER, int COUNT);
	void _Probe_Disks(void);
	bool IDE_SectorIO(uint8_t drive, bool write, uint64_t start, uint8_t count, void* Buffer);


	/* * * * * * * * * * * * * * * * * * * * * * * * * * *
	 *			   	   Local Variables					 *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * */

	#define _IDE_TIMEOUT 25//*~100mS = ~2500mS = ~2.5S

	#define DMA_CHANNEL 2
	#define IRQ_NUMBERa 14
	#define IRQ_NUMBERb 15

	#define IDE_ATA 0
	#define IDE_ATAPI 1
	#define ATA_MASTER 0
	#define ATA_SLAVE 1


	uint16_t *_IDE_BASE;

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

	typedef struct _IDE_DEVICES_STRUCT {
		bool Exists;      // 0 (Empty) or 1 (This Drive really exists).
		uint8_t Channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
		uint8_t Drive;       // 0 (Master Drive) or 1 (Slave Drive).
		unsigned short Type;        // 0: ATA, 1:ATAPI.
		unsigned short Signature;   // Drive Signature
		unsigned short Capabilities;// Features.
		unsigned int CommandSets; // Command Sets Supported.
		unsigned int Secotrs;     // Size in Sectors.
		uint8_t Model[41];   // Model in string.
	} _IDE_DRIVE;

#endif