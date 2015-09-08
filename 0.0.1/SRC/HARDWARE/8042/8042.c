/*
./DRIVERSRC/8042/8042.C
*/

#include "8042.H"
#include "../TIMER.H"
#include <STDIO.H>

void sendCommand(uint8_t data);
uint8_t getStatus(void);
void sendDevice(uint8_t data);
uint8_t getDevice(void);
bool readWait(void);
bool writeWait(void);

/*
LOCAL DATA
*/
enum _8042_Commands
{
	_8042_READBYTE = 0x20, //+0-1F
	_8042_WRITEBYTE = 0x60, //+0-1F
	_8042_DISABLE_PORT2 = 0xA7,
	_8042_ENABLE_PORT2 = 0xA8,
	_8042_TEST_PORT2 = 0xA9, //0 = PASS
	_8042_TEST = 0xAA, //0x55 = PASS - 0xFC = FAIL
	_8042_TEST_PORT1 = 0xAB, //0 = PASS
	_8042_DUMP = 0xAC,
	_8042_DISABLE_PORT1 = 0xAD,
	_8042_ENABLE_PORT1 = 0xAE,
	_8042_READ_INPUT_PORT = 0xC0,
	_8042_COPY_INPUT_LOW = 0xC1,
	_8042_COPY_INPUT_HIGH = 0xC2,
	_8042_READ_OUTPUT_PORT = 0xD0,
	_8042_WRITE_OUTPUT_PORT = 0xD1,
	_8042_WRITE_DEVICE_OUT_1 = 0xD2,
	_8042_WRITE_DEVICE_OUT_2 = 0xD3,
	_8042_WRITE_PORT2 = 0xD4		
};

enum _8042_Config
{
	_8042_PORT1_INT = 0x1, // 1 = ENABLE
	_8042_PORT2_INT = 0x2, // 1 = ENABLE
	_8042_SYS_FLAG = 0x4, // 1 IF PASSED BIOS POST
	_8042_PORT1_CLOCK = 0x10, // 0 = ENABLE
	_8042_PORT2_CLOCK = 0x20, // 0 = ENABLE
	_8042_PORT1_TRANSLATION = 0x40 // 1 = ENABLE
};

enum _8042_Ports
{
	_8042_DEVICE = 0x60,
	_8042_STATUS = 0x64,
	_8042_COMMAND = 0x64
};

enum _8042_Status
{
	_8042_OUTPUT_STATUS = 0x1,
	_8042_INPUT_STATUS = 0x2,
	_8042_FLAG = 0x4,
	_8042_CMD_DATA = 0x8,
	_8042_TIMEOUT = 0x40,
	_8042_PARITY =  0x80
};

bool dualPort = false;
bool initilized = false;
uint8_t CONFIG_8042;

/*
PRIVATE FUNCTIONS
*/
void sendCommand(uint8_t data)
{
	uint32_t _time_out=100000;
	while(_time_out--);
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 77) sendCommand(0x%x)\n\r", (uint32_t) data);
#endif
	outb(_8042_COMMAND, data);
}

uint8_t getStatus()
{
	uint32_t _time_out=100000;
	while(_time_out--);
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 85) getStatus()\n\r");
#endif
	return (uint8_t) inb(_8042_STATUS);
}

void sendDevice(uint8_t data)
{
	uint32_t _time_out=100000;
	while(_time_out--);
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 93) sendDevice(0x%x)\n\r", (uint32_t) data);
#endif
	outb(_8042_DEVICE, data);
}

uint8_t getDevice()
{
	uint32_t _time_out=100000;
	while(_time_out--);
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 101) getDevice()\n\r");
#endif
	return (uint8_t) inb(_8042_DEVICE);
}

bool readWait()
{
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 109) readWait()\n\r");
#endif
	uint32_t _time_out=100000;
	while(_time_out--)
		if ((getStatus() & _8042_OUTPUT_STATUS))
			return true;
	return false;
}

bool writeWait()
{
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 121) writeWait()\n\r");
#endif
	uint32_t _time_out=100000;
	while(_time_out--)
		if (!(getStatus() & _8042_INPUT_STATUS))
			return true;
	return false;
}

/*
PUBLIC FUNCTIONS
*/
uint8_t _8042_readPort()
{
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 136) _8042_readPort()\n\r");
#endif
	if (!readWait())
		return 0;
	return getDevice();
}

void _8042_writePort(int port, uint8_t data)
{
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 146) _8042_writePort(0x%x, 0x%x)\n\r", (uint32_t) port, (uint32_t) data);
#endif
	if (port == 2) {
		writeWait();
		sendCommand(_8042_WRITE_PORT2);
	}
	writeWait();
	sendDevice(data);
}

bool _8042_disablePort(int port)
{
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 159) _8042_disablePort(0x%x)\n\r", (uint32_t) port);
#endif
	if(port == 1) {
		sendCommand(_8042_DISABLE_PORT1);
		sendCommand(_8042_READBYTE);
		CONFIG_8042 = getDevice() & (0xFF - (_8042_PORT1_INT | _8042_PORT1_TRANSLATION));
		sendCommand(_8042_WRITEBYTE);
		sendDevice(CONFIG_8042);
		return true;
	} else if (port == 2) {
		sendCommand(_8042_DISABLE_PORT2);
		sendCommand(_8042_READBYTE);
		CONFIG_8042 = getDevice() & (0xFF - _8042_PORT2_INT);
		sendCommand(_8042_WRITEBYTE);
		sendDevice(CONFIG_8042);
		return true;
	} else 
		return false;
}

bool _8042_enablePort(int port)
{
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 182) _8042_enablePort(0x%x)\n\r", (uint32_t) port);
#endif
	if(port == 1) {
		sendCommand(_8042_ENABLE_PORT1);
		sendCommand(_8042_READBYTE);
		CONFIG_8042 = getDevice() | (_8042_PORT1_INT | _8042_PORT1_TRANSLATION);
		sendCommand(_8042_WRITEBYTE);
		sendDevice(CONFIG_8042);
		return true;
	} else if (port == 2) {
		sendCommand(_8042_ENABLE_PORT2);
		sendCommand(_8042_READBYTE);
		CONFIG_8042 = getDevice() | _8042_PORT2_INT;
		sendCommand(_8042_WRITEBYTE);
		sendDevice(CONFIG_8042);
		return true;
	} else
		return false;
}

bool _8042_init()
{
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 205) _8042_init()\n\r");
#endif
	// DISABLE 8042'S PORTS
	sendCommand(_8042_DISABLE_PORT1);
	sendCommand(_8042_DISABLE_PORT2);
	// FLUSH BUFFER
	while (getStatus() & _8042_OUTPUT_STATUS)
		getDevice();
	// GET CONFIG
	sendCommand(_8042_READBYTE);
	CONFIG_8042 = getDevice();
	// WE HAVE 2 PORTS?
	if (!(CONFIG_8042 & _8042_PORT2_CLOCK))
		dualPort = true;
	// CLEAR BIT 0, 1, 6
	CONFIG_8042 &= (_8042_SYS_FLAG | _8042_PORT1_CLOCK | _8042_PORT2_CLOCK);
	// SEND CONFIG
	sendCommand(_8042_WRITEBYTE);
	sendDevice(CONFIG_8042);
	// SELF TEST
	sendCommand(_8042_TEST);
	if (!getDevice() == 0x55) {	// ERROR 8042
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 229) 8042 Failed Test\n\r");
#endif
		return 0;
	}
	sendCommand(_8042_TEST_PORT1);
	if (getDevice()) {			// ERROR PORT ONE
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 236) 8042 Failed Port 1 Test\n\r");
#endif
		return 0;
	}
	sendCommand(_8042_TEST_PORT2);
	if (!getDevice())			// 2 PORTS
		return 2;
#ifdef DEBUG
	txf(1, "\n\r(8042.C:Line 242) 8042 Failed Port 2 Test\n\r");
#endif
	return 1;					// 1 PORT
}