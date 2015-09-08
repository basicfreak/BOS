/*
./DRIVERSRC/SYSTEM/CPU/INT.C
*/

#include "INT.H"
#include "IDT.H"
#include <STDIO.H>

void INT_HANDLER(regs *r);

void (*INTs[32])(regs *r) =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void install_INT(uint8_t intn, void (*handler)(regs *r))
{
    INTs[intn] = handler;
}

void uninstall_INT(uint8_t intn)
{
    INTs[intn] = 0;
}

// Common Handler for INTs
void INT_HANDLER(regs *r)
{
	void (*INT)(regs *r);
	// Get INTs Resolve Routine
	INT = INTs[r->int_no];
	// Do we have a valid Routine?
	if (INT)
		INT(r);
}

void _INT_init()
{
	install_Gate(0x30, (unsigned)INT0, 0x08, 0xEE);
    install_Gate(0x31, (unsigned)INT1, 0x08, 0xEE);
    install_Gate(0x32, (unsigned)INT2, 0x08, 0xEE);
    install_Gate(0x33, (unsigned)INT3, 0x08, 0xEE);
    install_Gate(0x34, (unsigned)INT4, 0x08, 0xEE);
    install_Gate(0x35, (unsigned)INT5, 0x08, 0xEE);
    install_Gate(0x36, (unsigned)INT6, 0x08, 0xEE);
    install_Gate(0x37, (unsigned)INT7, 0x08, 0xEE);

    install_Gate(0x38, (unsigned)INT8, 0x08, 0xEE);
    install_Gate(0x39, (unsigned)INT9, 0x08, 0xEE);
    install_Gate(0x3A, (unsigned)INT10, 0x08, 0xEE);
    install_Gate(0x3B, (unsigned)INT11, 0x08, 0xEE);
    install_Gate(0x3C, (unsigned)INT12, 0x08, 0xEE);
    install_Gate(0x3D, (unsigned)INT13, 0x08, 0xEE);
    install_Gate(0x3E, (unsigned)INT14, 0x08, 0xEE);
    install_Gate(0x3F, (unsigned)INT15, 0x08, 0xEE);

    install_Gate(0x40, (unsigned)INT16, 0x08, 0xEE);
    install_Gate(0x41, (unsigned)INT17, 0x08, 0xEE);
    install_Gate(0x42, (unsigned)INT18, 0x08, 0xEE);
    install_Gate(0x43, (unsigned)INT19, 0x08, 0xEE);
    install_Gate(0x44, (unsigned)INT20, 0x08, 0xEE);
    install_Gate(0x45, (unsigned)INT21, 0x08, 0xEE);
    install_Gate(0x46, (unsigned)INT22, 0x08, 0xEE);
    install_Gate(0x47, (unsigned)INT23, 0x08, 0xEE);

    install_Gate(0x48, (unsigned)INT24, 0x08, 0xEE);
    install_Gate(0x49, (unsigned)INT25, 0x08, 0xEE);
    install_Gate(0x4A, (unsigned)INT26, 0x08, 0xEE);
    install_Gate(0x4B, (unsigned)INT27, 0x08, 0xEE);
    install_Gate(0x4C, (unsigned)INT28, 0x08, 0xEE);
    install_Gate(0x4D, (unsigned)INT29, 0x08, 0xEE);
    install_Gate(0x4E, (unsigned)INT30, 0x08, 0xEE);
    install_Gate(0x4F, (unsigned)INT31, 0x08, 0xEE);
}