/*
./DRIVERSRC/SYSTEM/CPU/8259.H
*/

#ifndef				_8259_H_INCLUDED
	#define				_8259_H_INCLUDED
	
	#include <FORMATTING.H>
	
	void _8259_EOI(uint8_t IRQ);
	uint16_t _8259_Get_ISR(void);
	uint16_t _8259_Get_IRR(void);
	void _8259_Disable_IRQ(uint8_t IRQ);
	void _8259_Enable_IRQ(uint8_t IRQ);
	void _8259_INIT(void);
	
#endif