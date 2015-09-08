/*
./DRIVERSRC/SYSTEM/CPU/IRQ.C
*/

#include "IRQ.H"
#include "IDT.H"
#include <STDIO.H>
#include "8259.H"

void IRQ_HANDLER(regs *r);

void (*IRs[16])(regs *r) =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void install_IR(uint8_t irq, void (*handler)(regs *r))
{
    IRs[irq] = handler;
	_8259_Enable_IRQ(irq);
}

void uninstall_IR(uint8_t irq)
{
    IRs[irq] = 0;
	_8259_Disable_IRQ(irq);
}

void _IRQ_init()
{
    _8259_INIT();

    install_Gate(32, (unsigned)IRQ0, 0x08, 0x8E);
    install_Gate(33, (unsigned)IRQ1, 0x08, 0x8E);
    install_Gate(34, (unsigned)IRQ2, 0x08, 0x8E);
    install_Gate(35, (unsigned)IRQ3, 0x08, 0x8E);
    install_Gate(36, (unsigned)IRQ4, 0x08, 0x8E);
    install_Gate(37, (unsigned)IRQ5, 0x08, 0x8E);
    install_Gate(38, (unsigned)IRQ6, 0x08, 0x8E);
    install_Gate(39, (unsigned)IRQ7, 0x08, 0x8E);

    install_Gate(40, (unsigned)IRQ8, 0x08, 0x8E);
    install_Gate(41, (unsigned)IRQ9, 0x08, 0x8E);
    install_Gate(42, (unsigned)IRQ10, 0x08, 0x8E);
    install_Gate(43, (unsigned)IRQ11, 0x08, 0x8E);
    install_Gate(44, (unsigned)IRQ12, 0x08, 0x8E);
    install_Gate(45, (unsigned)IRQ13, 0x08, 0x8E);
    install_Gate(46, (unsigned)IRQ14, 0x08, 0x8E);
    install_Gate(47, (unsigned)IRQ15, 0x08, 0x8E);
	
	/// REMEMBER ALL IRQs ARE TURNED OFF!!!
	/// WE WILL ENABLE THEM WHEN INSTALLING HANDLERS
}

// Common Handler for IRQs 0-15
void IRQ_HANDLER(regs *r)
{
    void (*IR)(regs *r);
	// Get Interrupt Routine
    IR = IRs[r->int_no - 32];
	// Do we have a valid Interrupt Routine?
    if (IR)
    {
        IR(r);
    }
	// Send EOI
	_8259_EOI((uint8_t) (r->int_no - 32));
}
