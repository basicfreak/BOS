/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/FDC.C					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "FDC.H"
#include <STDIO.H>
#include "CMOS.H"
#include "DMA.H"
#include "../SYSTEM/CPU/IRQ.H"
#include "../SYSTEM/DEVICE/DISKMAN.H"
#include "TIMER.H"


///#define DEBUG

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   Declare Local Functions				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

error _FDC_SENSEINT(uint8_t *st0, uint8_t *cyl);
void _FDC_IR(regs *r);
error _FDC_WAIT_IR(void);
error _FDC_Write(uint16_t FDC, uint8_t OFF, uint8_t DATA);
error_data_u8_t _FDC_Read(uint16_t FDC, uint8_t OFF);
error DOR_Handler(uint8_t drive, bool motor, bool reset);
uint8_t MSR_Handler(void);
bool FDC_Ready(void);
error FDC_Specify(uint8_t drive, bool dma, uint8_t steprate, uint8_t loadtime, uint8_t unloadtime);
error FDC_Configure(uint8_t drive, bool impSeek, bool fifo, bool polling, uint8_t threshold);
error FDC_Calibrate(uint8_t drive);
error FDC_Speed(uint8_t drive, uint8_t speed);
error FDC_Reset(uint8_t drive);
error FDC_Seek(uint8_t drive, uint8_t head, uint8_t cylinder);
error FDC_SectorIO(uint8_t drive, bool write, uint8_t head, uint8_t track, uint8_t cylinder, uint8_t count);
void lbaCHS (int sector,int *head,int *track,int *cylinder);

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Variables					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool _FDC_IRQ;
bool _FDC_RESET_REQUIRED;

uint16_t timeout = 0xA0; // Timeouts This is times ~10mS (A0 = ~1.6S)

#define DMA_CHANNEL 2
#define IRQ_NUMBER 6

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

error _FDC_SENSEINT(uint8_t *st0, uint8_t *cyl)
{
#ifdef DEBUG
	txf(1, "_FDC_SENSEINT(*st0, *cyl)\n\r");
#endif
	error errorcode = ERROR_NONE;
	error_data_u8_t errordata = 0;
	if((errorcode = _FDC_Write(_FDC_BASE[0], DATA, _FDC_GETINT)))
		return errorcode;
	if(((errordata = _FDC_Read(_FDC_BASE[0], DATA)) >> 8))
		return (error) (errordata >> 8);
	*st0 = (uint8_t) (errordata & 0xFF);
	if(((errordata = _FDC_Read(_FDC_BASE[0], DATA)) >> 8))
		return (error) (errordata >> 8);
	*cyl = (uint8_t) (errordata & 0xFF);
	return ERROR_NONE;
}

error _FDC_WAIT_IR()
{
#ifdef DEBUG
	txf(1, "_FDC_WAIT_IR()\n\r");
#endif
	uint16_t i = timeout;
	while(!_FDC_IRQ && i--)
		timer_wait(1); //~10mS
	if(!i)
		return ERROR_TIMEOUT;
	_FDC_IRQ = false;
	return ERROR_NONE;
}

void _FDC_IR(regs *r)
{
#ifdef DEBUG
	txf(1, "\n\r\t\t\tFDC IRQ RECIVED\n\r");
#endif
	if(r->eax) {}
	_FDC_IRQ = true;
}

error _FDC_Write(uint16_t FDC, uint8_t OFF, uint8_t Data)
{
	if((FDC != 0x370 && FDC != 0x3F0) || OFF>CCR)
		return ERROR_INPUT;
	if(OFF != DATA)
		outb((uint16_t)(FDC+(uint16_t)OFF), Data);
	else {
		uint16_t i = timeout;
		while(!FDC_Ready() && i--)
			__asm__ __volatile__ ("nop");
		if(!i)
			return ERROR_TIMEOUT;
		outb((uint16_t)(FDC+(uint16_t)OFF), Data);
	}
#ifdef DEBUG
	txf(1, "_FDC_Write(0x%x, 0x%x, 0x%x)\n\r", FDC, OFF, Data);
#endif
	return ERROR_NONE;
}

error_data_u8_t _FDC_Read(uint16_t FDC, uint8_t OFF)
{
	uint8_t mydata;
	if((FDC != 0x370 && FDC != 0x3F0) || OFF>CCR)
		return (error_data_u8_t) (ERROR_INPUT << 8);
	if(OFF != DATA)
		mydata = inb((uint16_t)(FDC+(uint16_t)OFF));
	else {
		uint16_t i = timeout;
		while(!FDC_Ready() && i--)
			__asm__ __volatile__ ("nop");
		if(i)
			mydata = inb((uint16_t)(FDC+(uint16_t)OFF));
		else
			return (error_data_u8_t) (ERROR_TIMEOUT << 8);
	}
#ifdef DEBUG
	txf(1, "_FDC_READ(0x%x, 0x%x) = 0x%x\n\r", FDC, OFF, mydata);
#endif
	return (error_data_u8_t) ((ERROR_NONE << 8) | mydata);
}

error DOR_Handler(uint8_t drive, bool motor, bool reset)
{
#ifdef DEBUG
	txf(1, "DOR_Handler(0x%x, %s, %s)\n\r", drive, ((motor) ? "TRUE" : "FALSE"), ((reset) ? "TRUE" : "FALSE"));
#endif
	if(drive>3)
		return ERROR_INPUT;
	uint8_t Data = 0;
	if(!reset) {
		Data = 0xC;
		if(motor)
			Data |= (uint8_t) (1 << (4 + drive));
	}
	(void) _FDC_Write(_FDC_BASE[drive], DOR, Data); // I already handled the error check for DOR
	if (motor) timer_wait(15); // about 150 ms
	return ERROR_NONE;
}

uint8_t MSR_Handler()
{
	return (uint8_t) (_FDC_Read(_FDC_BASE[0], MSR) & 0xFF); // If there are errors here we are SOL anyways
}

bool FDC_Ready()
{
	if(MSR_Handler() & 0x80)
		return true;
	timer_wait(1); //~10mS
	return false;
}

error FDC_Specify(uint8_t drive, bool dma, uint8_t steprate, uint8_t loadtime, uint8_t unloadtime)
{
#ifdef DEBUG
	txf(1, "FDC_Specify(0x%x, %s, 0x%x, 0x%x, 0x%x)\n\r", drive, ((dma) ? "TRUE" : "FALSE"), steprate, loadtime, unloadtime);
#endif
	if(drive>3)
		return ERROR_INPUT;
	error errorcode = ERROR_NONE;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, _FDC_SPECIFY)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) (((steprate & 0xf) << 4) | (unloadtime & 0xf)))))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) ((loadtime << 1) | ((dma) ? 0 : 1)))))
		return errorcode;
	return ERROR_NONE;
}

error FDC_Configure(uint8_t drive, bool impSeek, bool fifo, bool polling, uint8_t threshold)
{
#ifdef DEBUG
	txf(1, "FDC_Configure(0x%x, %s, %s, %s, 0x%s)\n\r", drive, ((impSeek) ? "TRUE" : "FALSE"), ((fifo) ? "TRUE" : "FALSE"), ((polling) ? "TRUE" : "FALSE"), threshold );
#endif
	if(drive>3 || threshold>15)
		return ERROR_INPUT;
	uint8_t databyte = (uint8_t) (impSeek << 6) | threshold;
	if(!fifo)
		databyte |= (uint8_t) (1 << 5);
	if(!polling)
		databyte |= (uint8_t) (1 << 4);
	error errorcode = ERROR_NONE;
	if( (errorcode = _FDC_Write(_FDC_BASE[drive], DATA, 0x13)) )
		return errorcode;
	if( (errorcode = _FDC_Write(_FDC_BASE[drive], DATA, 0)) )
		return errorcode;
	if( (errorcode = _FDC_Write(_FDC_BASE[drive], DATA, databyte)) )
		return errorcode;
	if( (errorcode = _FDC_Write(_FDC_BASE[drive], DATA, 0)) )
		return errorcode;
	return ERROR_NONE;
}

error FDC_Calibrate(uint8_t drive)
{
#ifdef DEBUG
	txf(1, "FDC_Calibrate(0x%x)\n\r", drive);
#endif
	if(drive>3)
		return ERROR_INPUT;
	error errorcode = ERROR_NONE;
	if ( (errorcode = DOR_Handler(drive, true, false)) )
		return errorcode;
	uint8_t tries = 10;
	while(tries) {
		if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, _FDC_CALIBRATE)))
			return errorcode;
		if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, drive)))
			return errorcode;
		if((errorcode = _FDC_WAIT_IR()))
			return errorcode;
		uint8_t st0 = 0, cyl = 0;
		if((errorcode = _FDC_SENSEINT(&st0, &cyl)))
			return errorcode;
		if(!cyl) {
			if ( (errorcode = DOR_Handler(drive, false, false)) )
				return errorcode;
			return ERROR_NONE;
		}
		tries--;
	}
	return ERROR_HWFAILURE;
}

error FDC_Speed(uint8_t drive, uint8_t speed)
{
#ifdef DEBUG
	txf(1, "FDC_Speed(0x%x, 0x%x)\n\r", drive, speed);
#endif
	if(speed>3 || drive>3)
		return ERROR_INPUT;
	error errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], CCR, speed)))
		return errorcode;
	return ERROR_NONE;
}

error FDC_Reset(uint8_t drive)
{
#ifdef DEBUG
	txf(1, "FDC_Reset(0x%x)\n\r", drive);
#endif
	if(drive>3)
		return ERROR_INPUT;
	error errorcode = ERROR_NONE;
	if ( (errorcode = DOR_Handler(drive, false, true)) )				// Disable Controller
		return errorcode;
	if ( (errorcode = DOR_Handler(drive, true, false)) )				// Enable Controller
		return errorcode;
	if( (errorcode = FDC_Speed(drive, 0)) )								// Set speed to 500kb/s
		return errorcode;
	if( (errorcode = _FDC_WAIT_IR()) )									// Wait for IRQ
		return errorcode;
	uint8_t st0, cyl;
	for(int i=0; i<4; i++)
		if( (errorcode = _FDC_SENSEINT(&st0, &cyl)))					// Acknolage IRQ x4
			return errorcode;
	if( (errorcode = FDC_Configure(drive, false, true, false, 1)) )			//Configure
		return errorcode;
	if( (errorcode = FDC_Specify(drive, true, 8, 5, 15)) )				// Specify
		return errorcode;
	if( (errorcode = FDC_Calibrate(0)) )								// Calibrate (and turn off motor)
		return errorcode;
	_FDC_RESET_REQUIRED = false;										// We just reset so it is not required ATM
	return ERROR_NONE;
}

error FDC_Seek(uint8_t drive, uint8_t head, uint8_t cylinder)
{
#ifdef DEBUG
	txf(1, "FDC_Reset(0x%x, 0x%x, 0x%x)\n\r", drive, head, cylinder);
#endif
	uint8_t st0, cyl = 0;
	error errorcode = ERROR_NONE;
	for(int i = 0; i < 4; i ++) {
		if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, _FDC_SEEK)))
			return errorcode;
		if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) ((head << 2) | drive))))
			return errorcode;
		if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, cylinder)))
			return errorcode;
		if((errorcode = _FDC_WAIT_IR()))
			return errorcode;
		if((errorcode = _FDC_SENSEINT(&st0, &cyl)))
			return errorcode;
		if(cyl == cylinder)
			return ERROR_NONE;
	}
	return ERROR_HWFAILURE;
}

error FDC_SectorIO(uint8_t drive, bool write, uint8_t head, uint8_t track, uint8_t sector, uint8_t count)
{
#ifdef DEBUG
	txf(1, "FDC_SectorIO(0x%x, %s, 0x%x, 0x%x, 0x%x, 0x%x)\n\r", drive, ((write) ? "TRUE" : "FALSE"), head, track, sector, count);
#endif
	if(drive>3 || head>1 || track>79 || count>18)
		return ERROR_INPUT;
	uint8_t st0, cyl;
	error errorcode = ERROR_NONE;
	if((errorcode = _DMA_Setup(DMA_CHANNEL, write, 0x1000, (uint16_t)(count * 0x200))))
		return errorcode;
	if((errorcode = DOR_Handler(drive, true, false)))
		return errorcode;
	if((errorcode = FDC_Seek(drive, head, track)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, (_FDC_EXT_MULTITRACK | _FDC_EXT_DDENSITY | ((write) ? _FDC_WRITESECTOR : _FDC_READSECTOR) )    )))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, (uint8_t) ((head << 2) | drive))))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, track)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, head)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, sector)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, _DTL_512)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, count)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, 0x1B)))
		return errorcode;
	if((errorcode = _FDC_Write(_FDC_BASE[drive], DATA, 0xFF)))
		return errorcode;
	if((errorcode = _FDC_WAIT_IR()))
		return errorcode;
///printf("FDC IO RESPONSE: ");
	for(int i=0; i<7; i++)
	{
		error_data_u8_t errordata = _FDC_Read(_FDC_BASE[drive], DATA);
		if(errordata >> 8)
			return (error) (errordata >> 8);
///printf("0x%x ", errordata);
		if(!i && (errordata & 0xF8))
			_FDC_RESET_REQUIRED = true;
		if((i==1 || i==2) && (errordata & 0xFF))
			_FDC_RESET_REQUIRED = true;
	}
///printf("\n");
	if((errorcode = _FDC_SENSEINT(&st0, &cyl)))
		return errorcode;
	if((errorcode = DOR_Handler(drive, false, false)))
		return errorcode;
	return ERROR_NONE;
}

void lbaCHS (int sba,int *head,int *track,int *sector)
{
#ifdef DEBUG
	txf(1, "lbaCHS(");
#endif
	*head = ( sba % ( 18 * 2 ) ) / ( 18 );
	*track = sba / ( 18 * 2 );
	*sector = sba % 18 + 1;
#ifdef DEBUG
   txf(1, "0x%x, 0x%x, 0x%x, 0x%x)\n\r", sba, *head, *track, *sector);
#endif
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	  Public Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _FDC_init()
{
#ifdef DEBUG
	txf(1, "_FDC_init()\n\r");
#endif
	_FDC_RESET_REQUIRED = true;
	_FDC_IRQ = false;
	Install_Disk(0, 2880, &_FDC_IN, &_FDC_OUT);	//Install Disks in Disk Manager.
	install_IR(IRQ_NUMBER, _FDC_IR);
}

error _FDC_IO(bool write, int start, uint8_t count, void *buffer)
{
	uint8_t *MAINBUFFER = (uint8_t *) 0x1000;
	uint8_t *BUFFER = (uint8_t *) buffer;
	if (start >= 2880)
		return ERROR_INPUT;
	int tempstart = start;
	int head, track, sector;
	uint8_t tempcount = count;
	error temp;
	while( ((tempstart % 18) + tempcount) > 18 ) {
		if (_FDC_RESET_REQUIRED)
			FDC_Reset(0);
		lbaCHS(tempstart, &head, &track, &sector);
		if(write)		// Copy data into buffer
			memcpy(MAINBUFFER, &BUFFER[((tempstart-start)*0x200)], ((18-(tempstart % 18)) * 0x200));
		if((temp = FDC_SectorIO(0, write, (uint8_t) head, (uint8_t) track, (uint8_t) sector, (uint8_t)(18-(tempstart % 18)))))
			return temp;
		if(!write)		// Copy data out of buffer
			memcpy(&BUFFER[((tempstart-start)*0x200)], MAINBUFFER, ((18-(tempstart % 18)) * 0x200));
		tempcount = (uint8_t) (tempcount - (18-(tempstart%18)));
		tempstart += (18-(tempstart % 18));
	}
	if (_FDC_RESET_REQUIRED)
		FDC_Reset(0);
	lbaCHS(tempstart, &head, &track, &sector);
	if(write)
		memcpy(MAINBUFFER, &BUFFER[((tempstart-start)*0x200)], ((tempcount) * 0x200));
	if((temp = FDC_SectorIO(0, write, (uint8_t) head, (uint8_t) track, (uint8_t) sector, tempcount)))
		return temp;
	if(!write)
		memcpy(&BUFFER[((tempstart-start)*0x200)], MAINBUFFER, ((tempcount) * 0x200));
	return ERROR_NONE;
}


void _FDC_IN(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count)
{
	if(count > 0xFF) {
		*ERRORCODE = ERROR_INPUT;
		return;
	}
	if(LOCALID){};
	*ERRORCODE = _FDC_IO(false, (int) Start, (uint8_t) count, Buffer);
}

void _FDC_OUT(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count)
{
	if(count > 0xFF) {
		*ERRORCODE = ERROR_INPUT;
		return;
	}
	if(LOCALID){};
	*ERRORCODE = _FDC_IO(true, (int) Start, (uint8_t) count, Buffer);
}
