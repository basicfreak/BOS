/*
./DRIVERSRC/SYSTEM/CPU/8259.C
*/

#include "8259.H"
#include <stdio.h>

void _8259_Data_Out(bool secondary, uint8_t data);
void _8259_Command_Out(bool secondary, uint8_t data);
uint8_t _8259_Data_In(bool secondary);
uint8_t _8259_Command_In(bool secondary);
void _8259_Map(bool secondary, uint8_t addr);
void _8259_ICW3(bool secondary);
void _8259_ICW4(bool secondary);
void _8259_Disable_ALL_IRQ(void);
uint16_t _8259_Get_IRQ_Reg(uint8_t OCW3);


enum _8259_PORTS {
	Master_Command = 0x20,
	Master_Data = 0x21,
	Slave_Command = 0xA0,
	Slave_Data = 0xA1
};

enum _8259_COMMANDS {
	EOI = 0x20,
	INIT = 0x11,
	IRR = 0xA,
	ISR = 0xB
};

void _8259_Data_Out(bool secondary, uint8_t data)
{
	if (secondary)
		outb(Slave_Data, data);
	else
		outb(Master_Data, data);
}

void _8259_Command_Out(bool secondary, uint8_t data)
{
	if (secondary)
		outb(Slave_Command, data);
	else
		outb(Master_Command, data);
}

uint8_t _8259_Data_In(bool secondary)
{
	if (secondary)
		return inb(Slave_Data);
	else
		return inb(Master_Data);
}

uint8_t _8259_Command_In(bool secondary)
{
	if (secondary)
		return inb(Slave_Command);
	else
		return inb(Master_Command);
}

void _8259_Map(bool secondary, uint8_t addr)	//ICW2
{
	_8259_Data_Out(secondary, addr);
}

void _8259_ICW3(bool secondary)					//Connect PICs
{
	if (secondary)
		_8259_Data_Out(secondary, 2);
	else
		_8259_Data_Out(secondary, 4);
}

void _8259_ICW4(bool secondary)					//Using x86 not 80/85
{
	_8259_Data_Out(secondary, 1);
}

void _8259_Disable_ALL_IRQ()
{
	_8259_Data_Out(0, 0);
	_8259_Data_Out(1, 0);
}

void _8259_Disable_IRQ(uint8_t IRQ)
{
	uint8_t temp;
	if (IRQ < 8) {
		temp = _8259_Data_In(0);
		temp = (uint8_t) (temp | (1 << IRQ));
		_8259_Data_Out(0, temp);
	} else {
		temp = _8259_Data_In(1);
		temp = (uint8_t) (temp | (1 << (IRQ - 8)));
		_8259_Data_Out(1, temp);
	}
}

void _8259_Enable_IRQ(uint8_t IRQ)
{
	uint8_t temp;
	if (IRQ < 8) {
		temp = _8259_Data_In(0);
		temp = (uint8_t) (temp & ~(1 << IRQ));
		_8259_Data_Out(0, temp);
	} else {
		temp = _8259_Data_In(1);
		temp = (uint8_t) (temp & ~(1 << (IRQ - 8)));
		_8259_Data_Out(1, temp);
	}
}

uint16_t _8259_Get_IRQ_Reg(uint8_t OCW3)
{
	_8259_Command_Out(0, OCW3);
	_8259_Command_Out(1, OCW3);
	return (uint16_t) ((_8259_Command_In(1) << 8) | _8259_Command_In(0));
}

uint16_t _8259_Get_IRR()
{
	return _8259_Get_IRQ_Reg(IRR);
}

uint16_t _8259_Get_ISR()
{
	return _8259_Get_IRQ_Reg(ISR);
}

void _8259_EOI(uint8_t IRQ)
{
	if (IRQ > 7)
		_8259_Command_Out(1, EOI);
	if (IRQ < 16)						// Because I wanted no extra if's in IRQ.C
		_8259_Command_Out(0, EOI);
}

void _8259_INIT()
{
	// Send Initialize Command
	_8259_Command_Out(0, INIT);
	_8259_Command_Out(1, INIT);
	// Map IRQs (0-15) to 0x20-0x2F
	_8259_Map(0, 0x20);
	_8259_Map(1, 0x28);
	// Tell PICs how to talk
	_8259_ICW3(0);
	_8259_ICW3(1);
	// Tell PICs to use 80x86 mode
	_8259_ICW4(0);
	_8259_ICW4(1);
	// Disable All IRQs
	_8259_Disable_ALL_IRQ();
	_8259_Enable_IRQ(2);
}

