/*
./KERNELSRC/MAIN.C
*/

#include "../SYSTEM/CPU/GDT.H"
#include "../SYSTEM/CPU/IDT.H"
#include "../SYSTEM/CPU/IRQ.H"
#include "../SYSTEM/CPU/ISR.H"
#include "../SYSTEM/CPU/INT.H"
#include "../SYSTEM/MEM/VIRTUAL.H"
#include <STDIO.H>
#include <MATH.H>
#include "../HARDWARE/TIMER.H"
#include "../HARDWARE/CMOS.H"
#include "../HARDWARE/8042/KEYBOARD.H"
#include "../HARDWARE/8042/MOUSE.H"
#include "../HARDWARE/FDC.H"
#include "../HARDWARE/IDE.H"
#include "../SYSTEM/MEM/PHYSICAL.H"
#include "../SYSTEM/FS/VFS.H"
#include "../SYSTEM/FS/FAT.H"
#include "COMMAND.H"
#include "../HARDWARE/8042/8042.H"
#include "../HARDWARE/PCI.H"
#include "../HARDWARE/BIOS.H"
#include "../HARDWARE/RS232.H"
#include "../SYSTEM/CPU/8259.H"
#include "../SYSTEM/DEVICE/DISKMAN.H"
#include "../SYSTEM/API/API.H"
#include "../SYSTEM/API/THREADMAN.H"

extern uint32_t endkernel;

void load(const char* path);
extern void enter_usermode(void);
extern void install_tss (uint8_t idx, uint32_t kernelSS, uint32_t kernelESP);
extern void tss_set_stack (uint32_t kernelSS, uint32_t kernelESP);
void _exit_(void);
void _init(void);
void _init2(void);
int main(void);
void fpu_load_control_word(const uint16_t control);

void fpu_load_control_word(const uint16_t control)
{
    asm volatile("fldcw %0;"::"m"(control)); 
}

void _init()
{
	initVideo ();
puts("Installing GDT...");
	_GDT_init();
puts("Done.\nInstalling IDT...");
	_IDT_init();
puts("Done.\nInstalling ISR...");
	_ISR_init();
puts("Done.\nInstalling IRQ...");
	_IRQ_init();
puts("Done.\nInstalling INT...");
	_INT_init();
puts("Done.\nInstalling TSS...");
	install_tss(5,0x10,0);
puts("Done.\nInstalling API...");
	_API_init();
puts("Done.\nGetting BIOS Data...");
	_BIOS_init();
puts("Done.\nGetting CMOS Data...");
	readCMOS();
puts("Done.\nInitilizing PCI...");
	_PCI_init();
	__asm__ __volatile__ ("sti");						//DON'T FROGET TO RE-ENABLE INTS OR NOTHING WILL WORK RIGHT!!!!
puts("Done.\nInitilizing RS232...");
	_RS232_init();
fpu_load_control_word(0x37F);
fpu_load_control_word(0x37E);
fpu_load_control_word(0x37A);
puts("Done.\nInstalling Physical Memory Manager...");
	_PMem_init ();
puts("Done.\nInstalling Virtual Memory Manager...");
	_VMem_init();
puts("Done.\nInstalling Timer...");
	_TIMER_init();
puts("Done.\nInitilizing ThreadMan...");
	_THREAD_MAN_init();
}

void _init2()
{
puts("Done.\nInitilizing i8042...");
	bool _8042_init_stat = _8042_init();
	if (_8042_init_stat == 2) {
		puts("Done.\n\tInstalling PS/2 Keyboard...");
		_keyboard_init ();
		puts("Done.\n\tInstalling PS/2 Mouse...Done.\n");
		_mouse_init ();
	} else if (_8042_init_stat == 1) {
		puts("Warning.\n\tInstalling PS/2 Keyboard...");
		_keyboard_init ();
		puts("Done.\n\tWARNING:\tNO MOUSE\n");
	} else {
		puts("ERROR!\n\tWARNING:\tFORCING KEYBOARD...");
		_keyboard_init ();
		puts("Done.\n\tWARNING:\tNO MOUSE\n");
	}
puts("Initilizing Disk Manager...");
	_DISKMAN_init();
puts("Done.\nInitilizing Virtual File System...");
	_VFS_init();
puts("Done.\nInstalling FS FAT12/16/32...");
	_FAT_init();
//Any other File Systems Here
puts("Done.\nInstalling FDC...");
	_FDC_init();
puts("Done.\nInstalling IDE...");
	_IDE_init();
puts("DONE.\nEnabling User Land...");
	uint32_t EsP = 0;
	__asm__ __volatile__ ("mov %%esp, %0":"=g"(EsP));
	tss_set_stack(0x10, EsP);
puts("Done.\n");
printf("Found %i Disks with %i partitions\n", GetDiskCount(), GetPartitionCount());
printf("Kernel End = 0x%x\n", (uint32_t) &endkernel);
//load("clock.exe");
	init_cmd();
	while(CMD_ACTIVE)
		cmd_handler();
	_exit_();
}

void _exit_()
{
	movcur(ROWS - 1, COLS - 1);
	movcur_disable ++;
	setColor		(0x0F);
	cls();
	__asm__ __volatile__ ("cli");
	puts("So long, farewell\nAuf Wiedersehen, goodnight\n\n");
	puts("I hate to go and leave this pretty sight\n\nSo long, farewell\nAuf Wiedersehen, adieu\n\n");
	puts("Adieu, adieu\nTo you and you and you\n\n...\n\n");
	puts("The sun has gone\nTo bed and so must I\n\n");
	puts("So long, farewell\nAuf Wiedersehen, goodbye\n\n");
	puts("Goodbye\nGoodbye\nGoodbye\n\nGoodbye");
	setColor		(0x0E);
	curX = (COLS / 2) - 17;
	curY = ((ROWS - 1) / 2) - 1;
	puts("        So sad to see you go");
	curX = (COLS / 2) - 17;
	curY = ((ROWS + 1) / 2) - 1;
	puts("It is safe to turn off your computer");
	curX = (COLS / 2) - 17;
	curY = ROWS - 1;
	puts("             GOOD BYE!");
	__asm__ __volatile__ ("hlt");
	for(;;);
}

int main()
{
	_init();
	//KillThread(1);
	while(1);
	return 0;
}






	//__asm__ __volatile__ ("sti");
/*	sleep(60);				//wait 60 seconds and display EASTER EGG
	__asm__ __volatile__ ("cli");
	setColor (0x1F);
	cls();
	puts("So long and thanks for all the fish\n\
So sad that it should come to this\nWe tried to warn you all but oh dear?\n\n\
You may not share our intellect\nWhich might explain your disrespect\nFor all the natural wonders that grow around you\n\n\
So long, so long and thanks\nfor all the fish\n\n\
The world's about to be destroyed\nThere's no point getting all annoyed\nLie back and let the planet dissolve\n\n\
Despite those nets of tuna fleets\nWe thought that most of you were sweet\nEspecially tiny tots and your pregnant women\n\n\
So long, so long, so long, so long, so long\nSo long, so long, so long, so long, so long\n\n\
So long, so long and thanks\nfor all the fish\n");
	__asm__ __volatile__ ("hlt");
*/
	