/*
./DRIVERSRC/SYSTEM/CPU/INT.H
*/

#ifndef				INT_H_INCLUDED
	#define				INT_H_INCLUDED
	
	#include <FORMATTING.H>
	#include "IRQ.H"
	
	extern void INT0(void);
	extern void INT1(void);
	extern void INT2(void);
	extern void INT3(void);
	extern void INT4(void);
	extern void INT5(void);
	extern void INT6(void);
	extern void INT7(void);
	extern void INT8(void);
	extern void INT9(void);
	extern void INT10(void);
	extern void INT11(void);
	extern void INT12(void);
	extern void INT13(void);
	extern void INT14(void);
	extern void INT15(void);
	extern void INT16(void);
	extern void INT17(void);
	extern void INT18(void);
	extern void INT19(void);
	extern void INT20(void);
	extern void INT21(void);
	extern void INT22(void);
	extern void INT23(void);
	extern void INT24(void);
	extern void INT25(void);
	extern void INT26(void);
	extern void INT27(void);
	extern void INT28(void);
	extern void INT29(void);
	extern void INT30(void);
	extern void INT31(void);
	
	void _INT_init(void);
	void install_INT(uint8_t INTn, void (*handler)(regs *r));
	void uninstall_INT(uint8_t INTn);
	
#endif