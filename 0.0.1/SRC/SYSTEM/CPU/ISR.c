/*
./DRIVERSRC/SYSTEM/CPU/ISR.C
*/

#include "ISR.H"
#include "IDT.H"
#include "DISASM.H"
#include <STDIO.H>

void ISR_HANDLER(regs *r);
extern void KillCurrentThreadISRs(regs *r);

void (*ISRs[32])(regs *r) =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void install_ISR(uint8_t isr, void (*handler)(regs *r))
{
    ISRs[isr] = handler;
}

void uninstall_ISR(uint8_t isr)
{
    ISRs[isr] = 0;
}


void _ISR_init()
{
    install_Gate(0, (unsigned)ISR0, 0x08, 0x8E);
    install_Gate(1, (unsigned)ISR1, 0x08, 0x8E);
    install_Gate(2, (unsigned)ISR2, 0x08, 0x8E);
    install_Gate(3, (unsigned)ISR3, 0x08, 0x8E);
    install_Gate(4, (unsigned)ISR4, 0x08, 0x8E);
    install_Gate(5, (unsigned)ISR5, 0x08, 0x8E);
    install_Gate(6, (unsigned)ISR6, 0x08, 0x8E);
    install_Gate(7, (unsigned)ISR7, 0x08, 0x8E);

    install_Gate(8, (unsigned)ISR8, 0x08, 0x8E);
    install_Gate(9, (unsigned)ISR9, 0x08, 0x8E);
    install_Gate(10, (unsigned)ISR10, 0x08, 0x8E);
    install_Gate(11, (unsigned)ISR11, 0x08, 0x8E);
    install_Gate(12, (unsigned)ISR12, 0x08, 0x8E);
    install_Gate(13, (unsigned)ISR13, 0x08, 0x8E);
    install_Gate(14, (unsigned)ISR14, 0x08, 0x8E);
    install_Gate(15, (unsigned)ISR15, 0x08, 0x8E);

    install_Gate(16, (unsigned)ISR16, 0x08, 0x8E);
    install_Gate(17, (unsigned)ISR17, 0x08, 0x8E);
    install_Gate(18, (unsigned)ISR18, 0x08, 0x8E);
    install_Gate(19, (unsigned)ISR19, 0x08, 0x8E);
    install_Gate(20, (unsigned)ISR20, 0x08, 0x8E);
    install_Gate(21, (unsigned)ISR21, 0x08, 0x8E);
    install_Gate(22, (unsigned)ISR22, 0x08, 0x8E);
    install_Gate(23, (unsigned)ISR23, 0x08, 0x8E);

    install_Gate(24, (unsigned)ISR24, 0x08, 0x8E);
    install_Gate(25, (unsigned)ISR25, 0x08, 0x8E);
    install_Gate(26, (unsigned)ISR26, 0x08, 0x8E);
    install_Gate(27, (unsigned)ISR27, 0x08, 0x8E);
    install_Gate(28, (unsigned)ISR28, 0x08, 0x8E);
    install_Gate(29, (unsigned)ISR29, 0x08, 0x8E);
    install_Gate(30, (unsigned)ISR30, 0x08, 0x8E);
    install_Gate(31, (unsigned)ISR31, 0x08, 0x8E);
}

const char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// Common Handler for ISRSs
void ISR_HANDLER(regs *r)
{
	void (*ISR)(regs *r);
	// Get ISRS Resolve Routine
	ISR = ISRs[r->int_no];
	// Do we have a valid Routine?
	if (ISR)
		ISR(r);
	else
		iError(r);	// No Routine BSoD time.
}

void iError(regs *r)
{
	setColor (0x1F);
	cls ();
	puts ("\t\t\t\t\t\t\t   ");
	setColor (0xCF);
	puts ("Fatal System Error\n");
	txf(1, "Fatal System Error\n\r");
	setColor (0x1F);
	printf ("\t\t\t\t\t\t   %s Exception\n", exception_messages[r->int_no]);
	txf (1, "%s Exception\n\r", exception_messages[r->int_no]);
//HEX OF MEMORY LOCATIONS
	printf("EAX = 0x%x\tEBX = 0x%x\tECX = 0x%x\tEDX = 0x%x\n", r->eax, r->ebx, r->ecx, r->edx);
	printf("EDI = 0x%x\tESI = 0x%x\tEBP = 0x%x\tESP = 0x%x\n", r->edi, r->esi, r->ebp, r->esp);
	printf("GS = 0x%x\tFS = 0x%x\tES = 0x%x\tDS = 0x%x\n", r->gs, r->fs, r->es, r->ds);
	printf("EIP = 0x%x\tCS = 0x%x\tEFLAGS = 0x%x\tSS = 0x%x\n", r->eip, r->cs, r->eflags, r->ss);
	printf("USER-ESP = 0x%x\tERROR-CODE = 0x%x\n\n", r->useresp, r->err_code);
	txf(1, "EAX = 0x%x\tEBX = 0x%x\tECX = 0x%x\tEDX = 0x%x\n\r", r->eax, r->ebx, r->ecx, r->edx);
	txf(1, "EDI = 0x%x\tESI = 0x%x\tEBP = 0x%x\tESP = 0x%x\n\r", r->edi, r->esi, r->ebp, r->esp);
	txf(1, "GS = 0x%x\tFS = 0x%x\tES = 0x%x\tDS = 0x%x\n\r", r->gs, r->fs, r->es, r->ds);
	txf(1, "EIP = 0x%x\tCS = 0x%x\tEFLAGS = 0x%x\tSS = 0x%x\n\r", r->eip, r->cs, r->eflags, r->ss);
	txf(1, "USER-ESP = 0x%x\tERROR-CODE = 0x%x\n\r\n\r", r->useresp, r->err_code);
	uint32_t ramstart = (r->eip - 0x10);
	uint32_t ramend = (r->eip + 0x10);
	printf("RAM[0x%x - 0x%x]:\n", ramstart, ramend);
	txf(1, "RAM[0x%x - 0x%x]:\n\r", ramstart, ramend);
	uint8_t *RAMLOC = 0x0;
	for(uint32_t loc=ramstart; loc <= ramend; loc++)
	{
		if (loc != r->eip) {
			printf("%x ", RAMLOC[loc]);
			txf(1, "%x ", RAMLOC[loc]);
		} else {
			printfc(0x0F, "%x ", RAMLOC[loc]);
			txf(1, "(%x)  ", RAMLOC[loc]);
		}
	}
	putch('\n');
	putch('\n');
	txf(1, "\n\r\n\r");
	for(uint32_t loc=ramstart; loc <= ramend; loc++)
	{
		if (loc != r->eip) {
			printf("%s ", DisASM(RAMLOC[loc]));
			txf(1, "%s ", DisASM(RAMLOC[loc]));
		} else {
			printfc(0x0F, "%s ", DisASM(RAMLOC[loc]));
			txf(1, "(%s)  ", DisASM(RAMLOC[loc]));
		}
	}
	putch('\n');
	putch('\n');
	txf(1, "\n\r\n\r");
	for(uint32_t loc=ramstart; loc <= ramend; loc++)
	{
		if (loc != r->eip) {
			printf("%c ", RAMLOC[loc]);
			txf(1, "%c ", RAMLOC[loc]);
		} else {
			printfc(0x0F, "%c ", RAMLOC[loc]);
			txf(1, "(%c) ", RAMLOC[loc]);
		}
	}
	putch('\n');
	txf(1, "\n\r");
	if (r->cs == 0x08) {
		ramstart = (r->esp - 0x10);
		ramend = (r->esp + 0x10);
	} else {
		ramstart = (r->useresp - 0x10);
		ramend = (r->useresp + 0x10);
	}
	txf(1, "ESP - RAM[0x%x - 0x%x]:\n\r", ramstart, ramend);
	for(uint32_t loc=ramstart; loc <= ramend; loc++)
	{
		if (loc != r->eip)
			txf(1, "%x ", RAMLOC[loc]);
		else
			txf(1, "(%x)  ", RAMLOC[loc]);
	}
	txf(1, "\n\r\n\r");
	for(uint32_t loc=ramstart; loc <= ramend; loc++)
	{
		if (loc != r->eip)
			txf(1, "%c ", RAMLOC[loc]);
		else 
			txf(1, "(%c) ", RAMLOC[loc]);
	}
	txf(1, "\n\r");
//END HEX
	setColor (0x94);
	if (r->cs == 0x08) {		//KERNEL LEVEL
		puts ("\t\t\t\t\t\t\t\t System  Halted\n");
		txf (1, "System  Halted\n\r");
		__asm__ __volatile__ ("cli");
		__asm__ __volatile__ ("hlt");
		for (;;);
	} else {
		puts ("\t\t\t\t\t\t\t\t Attempting Restoration\n");
		txf (1, "Attempting Restoration\n\r");
		KillCurrentThreadISRs(r);
	}
}
