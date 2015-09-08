/*
./SRC/MAIN.c
*/

#include <hal.h>
#ifdef DEBUG_FULL
	#undef DEBUG_FULL
#endif
int main(void);
void Read(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
void Write(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
void _DMA_outb(uint16_t port, uint8_t data);
uint8_t _DMA_inb(uint16_t port);
bool _DMA_set_Addr(uint8_t channel, uint32_t addr);
bool _DMA_set_Count(uint8_t channel, uint16_t count);
bool _DMA_MASK_CHAN(uint8_t channel);
bool _DMA_UMASK_CHAN(uint8_t channel);
void _DMA_UMASK_ALL(bool dma);
bool _DMA_MODEREG_Handle(uint8_t channel, uint8_t mode);
void _DMA_RESET_FF(bool dma);
void _DMA_RESET(bool dma);

enum _DMA_PORTS {
	_DMA0_STATUS_REG = 0x8,			//Status Register(read)
	_DMA0_COMMAND_REG = 0x8,		//Command Register(write)
	_DMA0_REQUEST_REG = 0x9,		//Request Register(write)
	_DMA0_SINGMASK_REG = 0xa,		//Single Mask Register(write)
	_DMA0_MODE_REG = 0xb,			//Mode Register(write)
	_DMA0_CLEARBYTE_FF_REG = 0xc,	//Clear Byte Pointer Flip-Flop (write)
	_DMA0_TEMP_REG = 0xd,			//Intermediate Register(read)
	_DMA0_MASTER_CLEAR_REG = 0xd,	//Master Clear(write)
	_DMA0_CLEAR_MASK_REG = 0xe,		//Clear Mask Register(write)
	_DMA0_MASK_REG = 0xf,			//Write Mask Register(write)

	_DMA1_STATUS_REG = 0xd0,
	_DMA1_COMMAND_REG = 0xd0,
	_DMA1_REQUEST_REG = 0xd2,
	_DMA1_SINGMASK_REG = 0xd4,
	_DMA1_MODE_REG = 0xd6,
	_DMA1_CLEARBYTE_FF_REG = 0xd8,
	_DMA1_INTER_REG = 0xda,
	_DMA1_MASTER_CLEAR_REG = 0xda,
	_DMA1_UNMASK_ALL_REG = 0xdc,
	_DMA1_MASK_REG = 0xde
};

uint16_t _DMA_ADDR_REG[8] = { 0x0, 0x2, 0x4, 0x6, 0xC0, 0xC4, 0xC8, 0xCC };
uint16_t _DMA_COUNT_REG[8] = { 0x1, 0x3, 0x5, 0x7, 0xC2, 0xC6, 0xCA, 0xCE };
uint16_t _DMA_EXT_PAGE[8] = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };	//ASSUMING AT TYPE SYSTEM

enum _DMA_COMMAND_REG_MASK {
	_COMMAND_REG_MASK_MEMTOMEM = 1,
	_COMMAND_REG_MASK_CHAN0ADDRHOLD = 2,
	_COMMAND_REG_MASK_ENABLE = 4,
	_COMMAND_REG_MASK_TIMING = 8,
	_COMMAND_REG_MASK_PRIORITY = 0x10,
	_COMMAND_REG_MASK_WRITESEL = 0x20,
	_COMMAND_REG_MASK_DREQ = 0x40,
	_COMMAND_REG_MASK_DACK = 0x80
};

enum _DMA_MODE_REG_MASK {
	_DMA_MODE_SEL = 3,
	_DMA_MODE_TRA = 0xc,
	_DMA_MODE_SELF_TEST = 0,
	_DMA_MODE_READ_TRANSFER =4,
	_DMA_MODE_WRITE_TRANSFER = 8,
	_DMA_MODE_AUTO = 0x10,
	_DMA_MODE_IDEC = 0x20,
	_DMA_MODE = 0xc0,
	_DMA_MODE_TRANSFER_ON_DEMAND= 0,
	_DMA_MODE_TRANSFER_SINGLE = 0x40,
	_DMA_MODE_TRANSFER_BLOCK = 0x80,
	_DMA_MODE_TRANSFER_CASCADE = 0xC0
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
	MyHAL->flags = (HAL_F_PRESENT | HAL_F_READ | HAL_F_WRITE | HAL_F_HARDWARE);
	(void) strcpy((uint8_t *)MyHAL->name, (const uint8_t *)"DMA");
	(void) mkHAL(MyHAL);
	return 0;
}

void Read(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint8_t channel = (uint8_t) ((uint32_t)Address & 0xFF);
	uint32_t addr = (uint32_t) Buffer;
	uint16_t count = (uint16_t)(((uint32_t) Length) & 0xFFFF);
	if(channel>7 || addr>0xFF0000)
		return;
	_DMA_MASK_CHAN(channel);
	_DMA_MODEREG_Handle(channel, 0x46);
	_DMA_set_Addr(channel, addr);
	_DMA_set_Count(channel, count);
	_DMA_UMASK_CHAN(channel);
}

void Write(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint8_t channel = (uint8_t) ((uint32_t)Address & 0xFF);
	uint32_t addr = (uint32_t) Buffer;
	uint16_t count = (uint16_t) ((uint32_t)Length & 0xFFFF);
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__), __func__);
#endif
	if(channel>7 || addr>0xFF0000)
		return;
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__), __func__);
#endif
	_DMA_MASK_CHAN(channel);
	_DMA_MODEREG_Handle(channel, 0x4A);
	_DMA_set_Addr(channel, addr);
	_DMA_set_Count(channel, count);
	_DMA_UMASK_CHAN(channel);
}

void _DMA_outb(uint16_t port, uint8_t data)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	outb(port, data);
}

uint8_t _DMA_inb(uint16_t port)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	return inb(port);
}

bool _DMA_set_Addr(uint8_t channel, uint32_t addr)		//Sets all address data
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(channel > 7)
		return FALSE;
	if(addr > 0xFF0000)
		return FALSE;
	uint8_t ext_addr, high_addr, low_addr;
	low_addr = (uint8_t) (addr & 0xFF);
	high_addr = (uint8_t) ((addr >> 8) & 0xFF);
	ext_addr = (uint8_t) ((addr >> 16) & 0xFF);
	_DMA_outb(_DMA_ADDR_REG[channel], low_addr);
	_DMA_outb(_DMA_ADDR_REG[channel], high_addr);
	_DMA_outb(_DMA_EXT_PAGE[channel], ext_addr);
	return TRUE;
}

bool _DMA_set_Count(uint8_t channel, uint16_t count)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(channel > 7)
		return FALSE;
	count--;
	uint8_t high_count, low_count;
	low_count = (uint8_t) (count & 0xFF);
	high_count = (uint8_t) ((count >> 8) & 0xFF);
	_DMA_outb(_DMA_COUNT_REG[channel], low_count);
	_DMA_outb(_DMA_COUNT_REG[channel], high_count);
	return TRUE;
}

bool _DMA_MASK_CHAN(uint8_t channel)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(channel > 7)
		return FALSE;
	bool dma = ((channel < 4) ? false : true);
	uint8_t chan = ((dma) ? (uint8_t) (channel - 4) : channel);
	_DMA_outb(((dma) ? _DMA1_SINGMASK_REG : _DMA0_SINGMASK_REG), (uint8_t) (1 << chan));
	return TRUE;
}

bool _DMA_UMASK_CHAN(uint8_t channel)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(channel > 7)
		return FALSE;
	bool dma = ((channel < 4) ? false : true);
	uint8_t chan = ((dma) ? (uint8_t) (channel - 4) : channel);
	_DMA_outb(((dma) ? _DMA1_SINGMASK_REG : _DMA0_SINGMASK_REG), chan);
	return TRUE;
}

void _DMA_UMASK_ALL(bool dma)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	_DMA_outb(((dma) ? _DMA1_UNMASK_ALL_REG : _DMA0_CLEAR_MASK_REG), 0xFF);
}

bool _DMA_MODEREG_Handle(uint8_t channel, uint8_t mode)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	if(channel > 7)
		return FALSE;
	bool dma = ((channel < 4) ? false : true);
	uint8_t chan = ((dma) ? (uint8_t) (channel - 4) : channel);
	_DMA_outb(((dma) ? _DMA1_MODE_REG : _DMA0_MODE_REG), (chan | mode));
	return TRUE;
}

void _DMA_RESET_FF(bool dma)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	outb(((dma) ? _DMA1_CLEARBYTE_FF_REG : _DMA0_CLEARBYTE_FF_REG), 0xff);
}

void _DMA_RESET(bool dma)
{
#ifdef DEBUG_FULL
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	outb(((dma) ? _DMA1_MASTER_CLEAR_REG : _DMA0_MASTER_CLEAR_REG), 0xFF);
}
