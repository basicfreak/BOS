/*
./DRIVERSRC/SYSTEM/CPU/ISR.H
*/

#ifndef				ISR_H_INCLUDED
	#define				ISR_H_INCLUDED
	
	#include <FORMATTING.H>
	#include "IRQ.H"
	
	extern void ISR0(void);
	extern void ISR1(void);
	extern void ISR2(void);
	extern void ISR3(void);
	extern void ISR4(void);
	extern void ISR5(void);
	extern void ISR6(void);
	extern void ISR7(void);
	extern void ISR8(void);
	extern void ISR9(void);
	extern void ISR10(void);
	extern void ISR11(void);
	extern void ISR12(void);
	extern void ISR13(void);
	extern void ISR14(void);
	extern void ISR15(void);
	extern void ISR16(void);
	extern void ISR17(void);
	extern void ISR18(void);
	extern void ISR19(void);
	extern void ISR20(void);
	extern void ISR21(void);
	extern void ISR22(void);
	extern void ISR23(void);
	extern void ISR24(void);
	extern void ISR25(void);
	extern void ISR26(void);
	extern void ISR27(void);
	extern void ISR28(void);
	extern void ISR29(void);
	extern void ISR30(void);
	extern void ISR31(void);
	
	void _ISR_init(void);
	void install_ISR(uint8_t isrs, void (*handler)(regs *r));
	void uninstall_ISR(uint8_t isrs);
	void iError(regs *r);
	
#endif