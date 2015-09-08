/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/DMA.C					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "DMA.H"
#include <STDIO.H>

#include "TIMER.H"

///#define DEBUG

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   Declare Local Functions				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _DMA_outb(uint16_t port, uint8_t data);
uint8_t _DMA_inb(uint16_t port);
error _DMA_set_Addr(uint8_t channel, uint32_t addr);
error _DMA_set_Count(uint8_t channel, uint16_t count);
error _DMA_MASK_CHAN(uint8_t channel);
error _DMA_UMASK_CHAN(uint8_t channel);
void _DMA_UMASK_ALL(bool dma);
error _DMA_MODEREG_Handle(uint8_t channel, uint8_t mode);
void _DMA_RESET_FF(bool dma);
void _DMA_RESET(bool dma);

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Variables					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	   Local Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

void _DMA_outb(uint16_t port, uint8_t data)
{
#ifdef DEBUG
	txf(1, "_DMA_outb(0x%x, 0x%x)\n\r", port, data);
#endif
	outb(port, data);
}

uint8_t _DMA_inb(uint16_t port)
{
	uint8_t data = inb(port);
#ifdef DEBUG
	txf(1, "_DMA_inb(0x%x) = 0x%x\n\r", port, data);
#endif
	return data;
}

error _DMA_set_Addr(uint8_t channel, uint32_t addr)		//Sets all address data
{
	if(channel > 7)
		return ERROR_INPUT;
	if(addr > 0xFF0000)
		return ERROR_INPUT;
	uint8_t ext_addr, high_addr, low_addr;
	low_addr = (uint8_t) (addr & 0xFF);
	high_addr = (uint8_t) ((addr >> 8) & 0xFF);
	ext_addr = (uint8_t) ((addr >> 16) & 0xFF);
	_DMA_outb(_DMA_ADDR_REG[channel], low_addr);
	_DMA_outb(_DMA_ADDR_REG[channel], high_addr);
	_DMA_outb(_DMA_EXT_PAGE[channel], ext_addr);
	return ERROR_NONE;
}

error _DMA_set_Count(uint8_t channel, uint16_t count)
{
	if(channel > 7)
		return ERROR_INPUT;
	count--;
	uint8_t high_count, low_count;
	low_count = (uint8_t) (count & 0xFF);
	high_count = (uint8_t) ((count >> 8) & 0xFF);
	_DMA_outb(_DMA_COUNT_REG[channel], low_count);
	_DMA_outb(_DMA_COUNT_REG[channel], high_count);
	return ERROR_NONE;
}

error _DMA_MASK_CHAN(uint8_t channel)
{
	if(channel > 7)
		return ERROR_INPUT;
	bool dma = ((channel < 4) ? false : true);
	uint8_t chan = ((dma) ? (uint8_t) (channel - 4) : channel);
	_DMA_outb(((dma) ? _DMA1_SINGMASK_REG : _DMA0_SINGMASK_REG), (uint8_t) (1 << chan));
	return ERROR_NONE;
}

error _DMA_UMASK_CHAN(uint8_t channel)
{
	if(channel > 7)
		return ERROR_INPUT;
	bool dma = ((channel < 4) ? false : true);
	uint8_t chan = ((dma) ? (uint8_t) (channel - 4) : channel);
	_DMA_outb(((dma) ? _DMA1_SINGMASK_REG : _DMA0_SINGMASK_REG), chan);
	return ERROR_NONE;
}

void _DMA_UMASK_ALL(bool dma)
{
	_DMA_outb(((dma) ? _DMA1_UNMASK_ALL_REG : _DMA0_CLEAR_MASK_REG), 0xFF);
}

error _DMA_MODEREG_Handle(uint8_t channel, uint8_t mode)
{
	if(channel > 7)
		return ERROR_INPUT;
	bool dma = ((channel < 4) ? false : true);
	uint8_t chan = ((dma) ? (uint8_t) (channel - 4) : channel);
	_DMA_outb(((dma) ? _DMA1_MODE_REG : _DMA0_MODE_REG), (chan | mode));
	return ERROR_NONE;
}

void _DMA_RESET_FF(bool dma)
{
	outb(((dma) ? _DMA1_CLEARBYTE_FF_REG : _DMA0_CLEARBYTE_FF_REG), 0xff);
}

void _DMA_RESET(bool dma)
{
	outb(((dma) ? _DMA1_MASTER_CLEAR_REG : _DMA0_MASTER_CLEAR_REG), 0xFF);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			   	  Public Functions					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

error _DMA_Setup(uint8_t channel, bool write, uint32_t addr, uint16_t count)
{
	if(channel>7 || addr>0xFF0000)
		return ERROR_INPUT;
	error errorcode = ERROR_NONE;
	if((errorcode = _DMA_MASK_CHAN(channel)))
		return errorcode;
	if((errorcode = _DMA_MODEREG_Handle(channel, ((write) ? 0x4A : 0x46))))
		return errorcode;
	if((errorcode = _DMA_set_Addr(channel, addr)))
		return errorcode;
	if((_DMA_set_Count(channel, count)))
		return errorcode;
	if((errorcode = _DMA_UMASK_CHAN(channel)))
		return errorcode;
	return ERROR_NONE;
}
