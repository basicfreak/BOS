/*
./SRC/MAIN.c
*/

#include <hal.h>
#include <vfs.h>

#define CMOS 0x70
#define CMOSdata 0x71
#define DMA_CHANNEL 2
#define IRQ_NUMBER 6

#ifdef DEBUGA
	#define DEBUG_WAS
	#undef DEBUGA
#endif
#ifdef DEBUG_FULL
	#define DEBUG_FULL_WAS
	#undef DEBUG_FULL
#endif

int main(void);
void Read(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
void Write(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
void readCMOS(void);
void _DMA_Setup(uint8_t channel, bool write, uint32_t addr, uint16_t count);
bool _FDC_SENSEINT(uint8_t *st0, uint8_t *cyl);
void _FDC_IR(regs *r);
bool _FDC_WAIT_IR(void);
bool _FDC_Write(uint16_t FDC, uint8_t OFF, uint8_t DATA);
uint8_t _FDC_Read(uint16_t FDC, uint8_t OFF);
bool DOR_Handler(uint8_t drive, bool motor, bool reset);
uint8_t MSR_Handler(void);
bool FDC_Ready(void);
bool FDC_Specify(uint8_t drive, bool dma, uint8_t steprate, uint8_t loadtime, uint8_t unloadtime);
bool FDC_Configure(uint8_t drive, bool impSeek, bool fifo, bool polling, uint8_t threshold);
bool FDC_Calibrate(uint8_t drive);
bool FDC_Speed(uint8_t drive, uint8_t speed);
bool FDC_Reset(uint8_t drive);
bool FDC_Seek(uint8_t drive, uint8_t head, uint8_t cylinder);
bool FDC_SectorIO(uint8_t drive, bool write, uint8_t head, uint8_t track, uint8_t cylinder, uint8_t count);
void lbaCHS (int sector,int *head,int *track,int *cylinder);
bool _FDC_IO(bool write, int start, uint8_t count, void *buffer);

uint8_t CMOS_Floppy_Master, CMOS_Floppy_Slave;
uint16_t HAL_DMA = 0xFFFF;
bool _FDC_IRQ;
bool _FDC_RESET_REQUIRED;
uint16_t timeout = 0x18;//0x8; // Timeouts This is times ~10mS (A0 = ~1.6S)
static uint16_t _FDC_BASE[4] = { 0x3F0, 0x3F0, 0x370, 0x370 };

enum _FDC_PORT_BASES {
	_FDC_PRIMARY = 0x3f0,
	_FDC_SECONDARY = 0x370
};

enum _FDC_PORT_OFFSETS {
	STRA = 0,
	STRB = 1,
	DOR = 2,
	MSR = 4,
	DSR = 4,
	FIFO = 5,
	DATA = 5,
	DIR = 7,
	CCR = 7
};

enum _FDC_COMMANDS {
	_FDC_READTRACK = 0x2,
	_FDC_WRITESECTOR = 0x5,
	_FDC_READSECTOR = 0x6,
	_FDC_WRITEDELSECTOR = 0x9,
	_FDC_READDELSECTOR = 0xC,
	_FDC_FORMATTRACK = 0xD,

	_FDC_SPECIFY = 0x3,
	_FDC_DRIVESTAT = 0x4,
	_FDC_CALIBRATE = 0x7,
	_FDC_GETINT = 0x8,
	_FDC_READSECTORID = 0xA,
	_FDC_SEEK = 0xF,

	_FDC_VERSION = 0x10,

	_FDC_EXT_SKIP = 0x20,
	_FDC_EXT_DDENSITY = 0x40,
	_FDC_EXT_MULTITRACK = 0x80
};

enum _FDC_DTL {
 
	_DTL_128 = 0,
	_DTL_256 = 1,
	_DTL_512 = 2,
	_DTL_1024 = 4
};

int init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	HAL_p MyHAL = (HAL_p) calloc(sizeof(HAL_t));
	MyHAL->install = 0;
	MyHAL->read = Read;
	MyHAL->write = Write;
	MyHAL->remove = 0;
	MyHAL->flags = 0;
	MyHAL->limit = 0x168000;
	(void) strcpy((uint8_t *)MyHAL->name, (const uint8_t *)"FDC");
	uint16_t MyHALID = mkHAL(MyHAL);
	HAL_DMA = getHAL((char*)"DMA");
	_FDC_RESET_REQUIRED = TRUE;
	_FDC_IRQ = FALSE;
	newHandler(_FDC_IR, IRQ_NUMBER + 0x20);
	_8259_Enable_IRQ(IRQ_NUMBER);
//We will force this device to be a partition named FDC
	PART_p MyPart = (PART_p) calloc((sizeof(PART_t)));
	MyPart->HALid = MyHALID;
	MyPart->FSid = 1; // FAT12
	MyPart->Start = 0;
	MyPart->Length = 0x168000; // 1.44MB
	MyPart->PrivateID = 0;
	uint32_t MyPartID = mkPartition(MyPart);
	fmount(MyPartID);
	return 0;
}

void Read(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if((Length / 0x200) > 0xFF)
		return;
	(void) _FDC_IO(FALSE, ((int)Address), (uint8_t) (Length), Buffer);
}

void Write(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if((Length / 0x200) > 0xFF)
		return;
	(void) _FDC_IO(TRUE, ((int)Address), (uint8_t) (Length), Buffer);
}

void readCMOS()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	unsigned char temp;
	outb(CMOS, 0x10);
	temp = inb(CMOSdata);
	CMOS_Floppy_Master = temp >> 4;
	CMOS_Floppy_Slave = temp & 0xF;
}

void _DMA_Setup(uint8_t channel, bool write, uint32_t addr, uint16_t count)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(write)
		writeHAL(HAL_DMA, 0, (void*) ((uint32_t)channel), (void*) addr, (uint32_t)count);
	else
		readHAL(HAL_DMA, 0, (void*) ((uint32_t)channel), (void*) addr, (uint32_t)count);
}




bool _FDC_SENSEINT(uint8_t *st0, uint8_t *cyl)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(!_FDC_Write(_FDC_BASE[0], DATA, _FDC_GETINT))
		return FALSE;
	*st0 = _FDC_Read(_FDC_BASE[0], DATA);
	*cyl = _FDC_Read(_FDC_BASE[0], DATA);
	return TRUE;
}

bool _FDC_WAIT_IR()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint16_t i = timeout;
	while(!_FDC_IRQ && i--)
		PIT_wait(3);
		/*for(int to=0; to<0x7FFFFFFF; to++)
			__asm__ __volatile__("nop");*/
	if(!i)
		return FALSE;
	_FDC_IRQ = FALSE;
	return TRUE;
}

void _FDC_IR(regs *r)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(r->eax) {}
	_FDC_IRQ = true;
}

bool _FDC_Write(uint16_t FDC, uint8_t OFF, uint8_t Data)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if((FDC != 0x370 && FDC != 0x3F0) || OFF>CCR)
		return FALSE;
	if(OFF != DATA)
		outb((uint16_t)(FDC+(uint16_t)OFF), Data);
	else {
		uint16_t i = timeout;
		while(!FDC_Ready() && i--)
			PIT_wait(2);
			/*for(int to=0; to<0x7FFFFFFF; to++)
				__asm__ __volatile__("nop");*/
		if(!i)
			return FALSE;
		outb((uint16_t)(FDC+(uint16_t)OFF), Data);
	}
	return TRUE;
}

uint8_t _FDC_Read(uint16_t FDC, uint8_t OFF)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint8_t mydata;
	if((FDC != 0x370 && FDC != 0x3F0) || OFF>CCR)
		return 0;
	if(OFF != DATA)
		mydata = inb((uint16_t)(FDC+(uint16_t)OFF));
	else {
		uint16_t i = timeout;
		while(!FDC_Ready() && i--)
			PIT_wait(2);
			/*for(int to=0; to<0x7FFFFFFF; to++)
				__asm__ __volatile__("nop");*/
		if(i)
			mydata = inb((uint16_t)(FDC+(uint16_t)OFF));
		else
			return 0;
	}
	return mydata;
}

bool DOR_Handler(uint8_t drive, bool motor, bool reset)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(drive>3)
		return FALSE;
	uint8_t Data = 0;
	if(!reset) {
		Data = 0xC;
		if(motor)
			Data |= (uint8_t) (1 << (4 + drive));
	}
	(void) _FDC_Write(_FDC_BASE[drive], DOR, Data); // I already handled the error check for DOR
	if (motor)
		PIT_wait(50);
		/*for(uint32_t to=0; to<0x7FFFFFFF; to++)
			__asm__ __volatile__ ("nop");*/
	return TRUE;
}

uint8_t MSR_Handler()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	return (uint8_t) (_FDC_Read(_FDC_BASE[0], MSR) & 0xFF); // If there are errors here we are SOL anyways
}

bool FDC_Ready()
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(MSR_Handler() & 0x80)
		return true;
	return false;
}

bool FDC_Specify(uint8_t drive, bool dma, uint8_t steprate, uint8_t loadtime, uint8_t unloadtime)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(drive>3)
		return FALSE;
	if(!_FDC_Write(_FDC_BASE[drive], DATA, _FDC_SPECIFY))
		return FALSE;
	if(!_FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) (((steprate & 0xf) << 4) | (unloadtime & 0xf))))
		return FALSE;
	if(!_FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) ((loadtime << 1) | ((dma) ? 0 : 1))))
		return FALSE;
	return TRUE;
}

bool FDC_Configure(uint8_t drive, bool impSeek, bool fifo, bool polling, uint8_t threshold)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(drive>3 || threshold>15)
		return FALSE;
	uint8_t databyte = (uint8_t) (impSeek << 6) | threshold;
	if(!fifo)
		databyte |= (uint8_t) (1 << 5);
	if(!polling)
		databyte |= (uint8_t) (1 << 4);
	if( !_FDC_Write(_FDC_BASE[drive], DATA, 0x13) )
		return FALSE;
	if( !_FDC_Write(_FDC_BASE[drive], DATA, 0) )
		return FALSE;
	if( !_FDC_Write(_FDC_BASE[drive], DATA, databyte) )
		return FALSE;
	if( !_FDC_Write(_FDC_BASE[drive], DATA, 0) )
		return FALSE;
	return TRUE;
}

bool FDC_Calibrate(uint8_t drive)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(drive>3)
		return FALSE;
	if ( !DOR_Handler(drive, true, false) )
		return FALSE;
	uint8_t tries = 10;
	while(tries) {
		if(!_FDC_Write(_FDC_BASE[drive], DATA, _FDC_CALIBRATE))
			return FALSE;
		if(!_FDC_Write(_FDC_BASE[drive], DATA, drive))
			return FALSE;
		if(!_FDC_WAIT_IR())
			return FALSE;
		uint8_t st0 = 0, cyl = 0;
		if(!_FDC_SENSEINT(&st0, &cyl))
			return FALSE;
		if(!cyl) {
			if ( !DOR_Handler(drive, false, false) )
				return FALSE;
			return TRUE;
		}
		tries--;
	}
	return FALSE;
}

bool FDC_Speed(uint8_t drive, uint8_t speed)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if((speed<=3 || drive<=3) && (_FDC_Write(_FDC_BASE[drive], CCR, speed)))
		return TRUE;
	return FALSE;
}

bool FDC_Reset(uint8_t drive)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(drive>3)
		return FALSE;
	if ( !DOR_Handler(drive, false, true) )				// Disable Controller
		return FALSE;
	if ( !DOR_Handler(drive, true, false) )				// Enable Controller
		return FALSE;
	if( !FDC_Speed(drive, 0) )								// Set speed to 500kb/s
		return FALSE;
	if( !_FDC_WAIT_IR() )									// Wait for IRQ
		return FALSE;
	uint8_t st0, cyl;
	for(int i=0; i<4; i++)
		if( !_FDC_SENSEINT(&st0, &cyl))					// Acknolage IRQ x4
			return FALSE;
	if( !FDC_Configure(drive, false, true, false, 1) )			//Configure
		return FALSE;
	if( !FDC_Specify(drive, true, 8, 5, 15) )				// Specify
		return FALSE;
	if( !FDC_Calibrate(0) )								// Calibrate (and turn off motor)
		return FALSE;
	_FDC_RESET_REQUIRED = false;										// We just reset so it is not required ATM
	return TRUE;
}

bool FDC_Seek(uint8_t drive, uint8_t head, uint8_t cylinder)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint8_t st0, cyl = 0;
	for(int i = 0; i < 4; i ++) {
		if(!_FDC_Write(_FDC_BASE[drive], DATA, _FDC_SEEK))
			return FALSE;
		if(!_FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) ((head << 2) | drive)))
			return FALSE;
		if(!_FDC_Write(_FDC_BASE[drive], DATA, cylinder))
			return FALSE;
		if(!_FDC_WAIT_IR())
			return FALSE;
		if(!_FDC_SENSEINT(&st0, &cyl))
			return FALSE;
		if(cyl == cylinder)
			return TRUE;
	}
	return FALSE;
}

bool FDC_SectorIO(uint8_t drive, bool write, uint8_t head, uint8_t track, uint8_t sector, uint8_t count)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(drive>3 || head>1 || track>79)// || count>18)
		return FALSE;
	uint8_t st0, cyl;
	_DMA_Setup(DMA_CHANNEL, write, 0x1000, (uint16_t)(count * 0x200));
	if((!DOR_Handler(drive, true, false)))
		return FALSE;
	if((!FDC_Seek(drive, head, track)))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, (_FDC_EXT_MULTITRACK | _FDC_EXT_DDENSITY | ((write) ? _FDC_WRITESECTOR : _FDC_READSECTOR) )    )))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) ((head << 2) | drive))))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, track)))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, head)))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, sector)))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, _DTL_512)))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, count)))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, 0x1B)))
		return FALSE;
	if((!_FDC_Write(_FDC_BASE[drive], DATA, 0xFF)))
		return FALSE;
	if((!_FDC_WAIT_IR()))
		return FALSE;
///printf("FDC IO RESPONSE: ");
	for(int i=0; i<7; i++)
	{
		uint8_t data = _FDC_Read(_FDC_BASE[drive], DATA);
		
///printf("0x%x ", errordata);
		if(!i && (data & 0xF8))
			_FDC_RESET_REQUIRED = true;
		if((i==1 || i==2) && data)
			_FDC_RESET_REQUIRED = true;
	}
///printf("\n");
	if((!_FDC_SENSEINT(&st0, &cyl)))
		return FALSE;
	if((!DOR_Handler(drive, false, false)))
		return FALSE;
	return TRUE;
}

void lbaCHS (int sba,int *head,int *track,int *sector)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	*head = ( sba % ( 18 * 2 ) ) / ( 18 );
	*track = sba / ( 18 * 2 );
	*sector = sba % 18 + 1;
}

bool _FDC_IO(bool write, int start, uint8_t count, void *buffer)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint8_t *MAINBUFFER = (uint8_t *) 0x1000;
	uint8_t *BUFFER = (uint8_t *) buffer;
	if (start >= 2880)
		return FALSE;
	int tempstart = start;
	int head, track, sector;
	uint8_t tempcount = count;

	while(tempcount) {
		lbaCHS(tempstart, &head, &track, &sector);
#ifdef DEBUG
	DEBUG_printf("\tHead %i\tTrack %i\tSector %i\n", (uint8_t) head, (uint8_t) track, (uint8_t) sector);
#endif
		// if(_FDC_RESET_REQUIRED)
			FDC_Reset(0);
		if(write)		// Copy data into buffer
			memcpy(MAINBUFFER, &BUFFER[((tempstart - start)*0x200)], 0x200);
		if(!FDC_SectorIO(0, write, (uint8_t) head, (uint8_t) track, (uint8_t) sector, 1))
			return FALSE;
		if(!write)		// Copy data out of buffer
			memcpy(&BUFFER[((tempstart - start)*0x200)], MAINBUFFER, 0x200);
#ifdef DEBUG_FULL		
	DEBUG_printf("\n");
	for(int x = 0; x < 0x200; x++)
		DEBUG_printf("%x ", MAINBUFFER[x]);
	DEBUG_printf("\n");
#endif
		tempstart ++;
		tempcount --;
	}
	return TRUE;
}

/*bool _FDC_IO(bool write, int start, uint8_t count, void *buffer)
{
	uint8_t *MAINBUFFER = (uint8_t *) 0x1000;
	uint8_t *BUFFER = (uint8_t *) buffer;
	if (start >= 2880)
		return FALSE;
	int tempstart = start;
	int head, track, sector;
	uint8_t tempcount = count;

	while(tempcount) {
		lbaCHS(tempstart, &head, &track, &sector);
		//uint8_t thismax = 18 - ((tempstart+1) % 18); // Base is 0
		uint8_t thismax = 18-(((uint8_t)sector)); // Base is 1
		if(!(uint8_t)head)
			thismax += 18;
DEBUG_printf("\ttempstart = 0x%x, tempcount = 0x%x, thismax = 0x%x\n", tempstart, tempcount, thismax);
DEBUG_printf("0x%x, 0x%x, 0x%x\n", ((tempstart - start)*0x200), (thismax * 0x200), (tempcount * 0x200));
DEBUG_printf("%i\t%i\t%i\n", (uint8_t) head, (uint8_t) track, (uint8_t) sector);
		FDC_Reset(0);
		if(thismax <= tempcount) {
			if(write)		// Copy data into buffer
				memcpy(MAINBUFFER, &BUFFER[((tempstart - start)*0x200)], (thismax * 0x200));
			if(!FDC_SectorIO(0, write, (uint8_t) head, (uint8_t) track, (uint8_t) sector, thismax+1))
				return FALSE;
			if(!write)		// Copy data out of buffer
				memcpy(&BUFFER[((tempstart - start)*0x200)], MAINBUFFER, (thismax * 0x200));
			tempcount -= thismax;
			tempstart += (thismax+1);
		} else {
			if(write)		// Copy data into buffer
				memcpy(MAINBUFFER, &BUFFER[((tempstart - start)*0x200)], (tempcount * 0x200));
			if(!FDC_SectorIO(0, write, (uint8_t) head, (uint8_t) track, (uint8_t) sector, tempcount+1))
				return FALSE;
			if(!write)		// Copy data out of buffer
				memcpy(&BUFFER[((tempstart - start)*0x200)], MAINBUFFER, (tempcount * 0x200));
			tempstart += tempcount;
			tempcount -= tempcount;
		}
	}

	return TRUE;
}*/

/*bool _FDC_IO(bool write, int start, uint8_t count, void *buffer)
{
	uint8_t *MAINBUFFER = (uint8_t *) 0x1000;
	uint8_t *BUFFER = (uint8_t *) buffer;
	if (start >= 2880)
		return FALSE;
	int tempstart = start;
	int head, track, sector;
	uint8_t tempcount = count;
	while( ((tempstart % 18) + tempcount) > 18 ) {
		// if (_FDC_RESET_REQUIRED)
			FDC_Reset(0);
		lbaCHS(tempstart, &head, &track, &sector);
		if(write)		// Copy data into buffer
			memcpy(MAINBUFFER, &BUFFER[((tempstart-start)*0x200)], ((18-(tempstart % 18)) * 0x200));
		if(!FDC_SectorIO(0, write, (uint8_t) head, (uint8_t) track, (uint8_t) sector, (uint8_t)(18-(tempstart % 18))))
			return FALSE;
		if(!write)		// Copy data out of buffer
			memcpy(&BUFFER[((tempstart-start)*0x200)], MAINBUFFER, ((18-(tempstart % 18)) * 0x200));
		tempcount = (uint8_t) (tempcount - (18-(tempstart%18)));
		tempstart += (18-(tempstart % 18));
	}
	// if(_FDC_RESET_REQUIRED)
		FDC_Reset(0);
	lbaCHS(tempstart, &head, &track, &sector);
	if(write)
		memcpy(MAINBUFFER, &BUFFER[((tempstart-start)*0x200)], ((tempcount) * 0x200));
	if(!FDC_SectorIO(0, write, (uint8_t) head, (uint8_t) track, (uint8_t) sector, tempcount))
		return FALSE;
	if(!write)
		memcpy(&BUFFER[((tempstart-start)*0x200)], MAINBUFFER, ((tempcount) * 0x200));
	return TRUE;
}*/



#ifdef DEBUG_WAS
	#undef DEBUG_WAS
	#define DEBUG
#endif
#ifdef DEBUG_FULL_WAS
	#undef DEBUG_FULL_WAS
	#define DEBUG_FULL
#endif
	