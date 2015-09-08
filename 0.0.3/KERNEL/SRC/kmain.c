#include <BOSBOOT.h>
#include <i386/cpu/gdt.h>
#include <i386/cpu/idt.h>
#include <i386/cpu/8259.h>
#include <i386/cpu/tss.h>
#include <i386/cpu/sse.h>
#include <i386/memory/physman.h>
#include <i386/memory/virtman.h>
#include <i386/memory/heapman.h>
#include <i386/hardware/pit/pit.h>
#include <api/api.h>
#include <api/modules.h>
#include <hal.h>
#include <vfs.h>

int kmain(BootInfo_p BOOTINF);

int kmain(BootInfo_p BOOTINF)
{
#ifdef DEBUG
	DEBUG_print("Checking Boot Info...");
#endif
	if(BOOTINF->KernelStart != 0x100000) {
#ifdef DEBUG
	DEBUG_print("ERROR.\n");
#endif
		return 0;
	}

#ifdef DEBUG
	DEBUG_print(".DONE!\nInstalling GDT...");
#endif
	_GDT_init();
#ifdef DEBUG
	DEBUG_print(".DONE!\nInitilizing 8259...");
#endif
	_8259_init();
#ifdef DEBUG
	DEBUG_print(".DONE!\nInstalling IDT...");
#endif
	_IDT_init();
#ifdef DEBUG
	DEBUG_print(".DONE!\nInstalling TSS...");
#endif
	_TSS_init();
#ifdef DEBUG
	DEBUG_print(".DONE!\nEnableing SSE...");
#endif
	_SSE_init();
#ifdef DEBUG
	DEBUG_print(".DONE!\nSetting IF...");
#endif
	sti();
#ifdef DEBUG
	DEBUG_print(".DONE!\nSetting up Physical Memory Manager...");
#endif
	_PMM_init(BOOTINF);
#ifdef DEBUG
	DEBUG_print(".DONE!\nSetting up Virtual Memory Manager...");
#endif
	_VMM_init(BOOTINF);
// #ifdef DEBUG
// 	DEBUG_print(".DONE!\nSetting up Heap Memory Manager...");
// #endif
// 	_HMM_init();
/*#ifdef DEBUG
	DEBUG_print(".DONE!\nSetting up Thread Manager...");
#endif
	_TM_init();
*/
#ifdef DEBUG
	DEBUG_print(".DONE!\nInitilizing PIT...");
#endif
	_PIT_init();
#ifdef DEBUG
	DEBUG_print(".DONE!\nSetting Up Kernel API...");
#endif
	_API_init();
	_API_Add_Symbol("BOOTINF", (uint32_t)BOOTINF);
	_API_Add_Symbol("PIT_wait", (uint32_t)&PIT_wait);
	_API_Add_Symbol("PIT_Ticks", (uint32_t)&PIT_Ticks);
	_API_Add_Symbol("sleep", (uint32_t)&sleep);
	_API_Add_Symbol("_8259_Enable_IRQ", (uint32_t)&_8259_Enable_IRQ);
#ifdef DEBUG
	DEBUG_print(".DONE!\nInitilizing HAL...");
#endif
	_HAL_init();
#ifdef DEBUG
	DEBUG_print(".DONE!\nInitilizing VFS...");
#endif
	_VFS_init();
/*	for(;;) {
		sleep(1);
		DEBUG_printf("1 Second. ");
	}*/
#ifdef DEBUG
	DEBUG_print(".DONE!\nLoading Boot Time Modules...");
#endif
	_BTM_init(BOOTINF);
#ifdef DEBUG
	DEBUG_print(".DONE!\n");
#endif

void *NEWBS = calloc(512);
void *HDDBS = calloc(512);
uint32_t HDDPID = GetPartByName("BOSHDD     ");
FILE *F16BS = fopen("/BOS v 0.0.3/vbr.bin");
if(F16BS->Length == 0x200 && HDDPID != 0xFFFFFFFF) {
	PartRead(HDDPID, 0, HDDBS, 1);
	fread(F16BS, 0, NEWBS, 0x200);
	memcpy(HDDBS, NEWBS, 3);
	memcpy((void*)(((uint32_t)HDDBS) + 0x3E), (void*)(((uint32_t)NEWBS) + 0x3E), 0x1C2);
	PartWrite(HDDPID, 0, HDDBS, 1);
} else {
	DEBUG_printf("ERROR BOOTSECTOR");
	for(;;);
}
fclose(F16BS);
free(HDDBS, 512);
free(NEWBS, 512);

/*FILE *StageTwo = fopen("/BOS v 0.0.3/oslder.sys");
if(StageTwo->Length) {
	FILE *OF = fnew("/BOSHDD     /osloader.sys");
	void *Buffer = calloc(StageTwo->Length);
	fread(StageTwo, 0, Buffer, StageTwo->Length);
	fwrite(OF, 0, Buffer, StageTwo->Length);
	fclose(OF);
} else {
	DEBUG_printf("ERROR OSLOADER");
	for(;;);
}
fclose(StageTwo);*/

for(;;);
/*
FILE *StageTwo = fopen("/BOS v 0.0.3/stagetwo.bin");
if(StageTwo->Length) {
	FILE *OF = fnew("/BOSHDD     /stagetwo.bin");
	void *Buffer = calloc(StageTwo->Length);
	fread(StageTwo, 0, Buffer, StageTwo->Length);
	fwrite(OF, 0, Buffer, StageTwo->Length);
	fclose(OF);
}
fclose(StageTwo);
fnewDIR("/BOSHDD     /boot");
fnewDIR("/BOSHDD     /bin");
FILE *KernelFile = fopen("/BOS v 0.0.3/bin/kernel.elf");
if(KernelFile->Length) {
	FILE *OF = fnew("/BOSHDD     /bin/kernel.elf");
	void *Buffer = calloc(KernelFile->Length);
	fread(KernelFile, 0, Buffer, KernelFile->Length);
	fwrite(OF, 0, Buffer, KernelFile->Length);
	fclose(OF);
}
fclose(KernelFile);
FILE *Svga = fopen("/BOS v 0.0.3/bin/svga.dll");
if(Svga->Length) {
	FILE *OF = fnew("/BOSHDD     /bin/svga.dll");
	void *Buffer = calloc(Svga->Length);
	fread(Svga, 0, Buffer, Svga->Length);
	fwrite(OF, 0, Buffer, Svga->Length);
	fclose(OF);
}
fclose(Svga);
FILE *Cli = fopen("/BOS v 0.0.3/bin/cli.dll");
if(Cli->Length) {
	FILE *OF = fnew("/BOSHDD     /bin/cli.dll");
	void *Buffer = calloc(Cli->Length);
	fread(Cli, 0, Buffer, Cli->Length);
	fwrite(OF, 0, Buffer, Cli->Length);
	fclose(OF);
}
fclose(Cli);
FILE *Boot = fopen("/BOS v 0.0.3/boot/boot.cfg");
if(Boot->Length) {
	FILE *OF = fnew("/BOSHDD     /boot/boot.cfg");
	void *Buffer = calloc(Boot->Length);
	fread(Boot, 0, Buffer, Boot->Length);
	fwrite(OF, 0, Buffer, Boot->Length);
	fclose(OF);
}
fclose(Boot);*/

/*uint32_t PartIDFDD = GetPartByName("BOS v 0.0.3");
uint32_t PartIDHDD = GetPartByName("BOSHDD     ");

void *BootSectorFDD = calloc(512);
void *BootSectorHDD = calloc(512);
PartRead(PartIDFDD, 0, BootSectorFDD, 1);
PartRead(PartIDHDD, 0, BootSectorHDD, 1);

memcpy(BootSectorHDD, BootSectorFDD, 3);
memcpy((void*)(((uint32_t)BootSectorHDD) + 0x3E),  (void*)(((uint32_t)BootSectorFDD) + 0x3E), 0x1C2);

PartWrite(PartIDHDD, 0, BootSectorHDD, 1);

free(BootSectorHDD, 512);
free(BootSectorFDD, 512);
	
	FILE *MyTestFile = fopen("/BOSHDD     /test.txt");
	if(MyTestFile->Length) {
		char *MyBuff = (char*) calloc(MyTestFile->Length);
		fread(MyTestFile, 0, MyBuff, MyTestFile->Length);
		DEBUG_print("\n");
		for(int x = 0; x < (int) MyTestFile->Length; x++)
			DEBUG_printf("%c", MyBuff[x]);
		DEBUG_print("\n");
		free(MyBuff, MyTestFile->Length);
		fclose(MyTestFile);
	}

DEBUG_printf("\nCOPPYING SVGA TEST FROM FLOPPY TO HDD PARTITION 1\n");

FILE *IF = fopen("/BOS v 0.0.3/bin/svga.dll");
if(IF->Length) {
	fnewDIR("/BOSEXT     /bin");
	FILE *OF = fnew("/BOSEXT     /bin/svga.dll");
	if(OF->Start) {
		void* Buffer = calloc(IF->Length);
		fread(IF, 0, Buffer, IF->Length);
		fwrite(OF, 0, Buffer, IF->Length);
	}
	fclose(OF);
}
fclose(IF);


DEBUG_printf("\nLOADING SVGA TEST FROM HDD PARTITION 1\n");

FILE *VGAFILE = fopen("/BOSEXT     /bin/svga.dll");
if(VGAFILE->Length) {
	void* Buffer = calloc(IF->Length);
	fread(VGAFILE, 0, Buffer, (VGAFILE->Length));
	for(uint32_t x = 0; x < ((800*600*2)+0x1000); x+=0x1000)
		_VMM_map((BOOTINF->FrameBuffer + x), (0x200000 + x), FALSE, TRUE);
	BOOTINF->FrameBuffer = 0x200000;

	Load_Mod(Buffer);

	free(Buffer, VGAFILE->Length);
}
fclose(VGAFILE);
// fclose(OF);

	for(;;)
		hlt();*/
/*
#ifdef DEBUG
	DEBUG_print("Loading PCI Module...");
#endif
	FILE *MyFile = fopen("/FDC/bin/pci.dll");
	if(MyFile->Length) {
		void* MyFileBuff = calloc(MyFile->Length);
		fread(MyFile, 0, MyFileBuff, MyFile->Length);
		Load_Mod(MyFileBuff);
		free(MyFileBuff, MyFile->Length);
		fclose(MyFile);
	#ifdef DEBUG
		DEBUG_print(".DONE!\n");
	#endif
	} else {
	#ifdef DEBUG
		DEBUG_print(".ERROR!\n");
	#endif
	}
*/

	return 0;
}