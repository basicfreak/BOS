/*
./DRIVERSRC/SYSTEM/CPU/IRQ.H
*/

#ifndef				IRQ_H_INCLUDED
	#define				IRQ_H_INCLUDED
	
	#include <FORMATTING.H>

	extern void IRQ0(void);
	extern void IRQ1(void);
	extern void IRQ2(void);
	extern void IRQ3(void);
	extern void IRQ4(void);
	extern void IRQ5(void);
	extern void IRQ6(void);
	extern void IRQ7(void);
	extern void IRQ8(void);
	extern void IRQ9(void);
	extern void IRQ10(void);
	extern void IRQ11(void);
	extern void IRQ12(void);
	extern void IRQ13(void);
	extern void IRQ14(void);
	extern void IRQ15(void);
	
	typedef struct registers	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;    
	} regs;
	
	void install_IR(uint8_t irq, void (*handler)(regs *r));
	void uninstall_IR(uint8_t irq);
	void _IRQ_init(void);
	
#endif