/*
./SRC/COMMAND.C
*/

#include <MATH.H>
#include "COMMAND.H"
#include <STRING.H>
#include <STDIO.H>
#include "../HARDWARE/FDC.H"
#include "../HARDWARE/IDE.H"
#include "../HARDWARE/TIMER.H"
#include "../SYSTEM/API/THREADMAN.H"
#include "../SYSTEM/FS/VFS.H"
#include "../HARDWARE/PCI.H"
#include "../SYSTEM/CPU/INT.H"
#include "../SYSTEM/DEVICE/DISKMAN.H"

extern void enter_usermode(void);

#define DEBUG

void cmd_read(uint16_t partid, uint64_t start, uint16_t count);
//void cat(const char* path);
//void load(const char* path);
void help(void);
void do_CMD(int args);
void CopyFloppyTest(void);

char *CurrentPath;
//char cmd_command[1024];
char *cmd_command;
//char explode_cmd[50][100];
char *explode_cmd[256];
uint8_t *readbuffer;
void help()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 31) help()\n\r");
#endif
	puts ("MyDOS v. 0.1\n");
	puts ("-------------------------------------\n");
	//puts ("cat [PATH]      Reads File off disk\n");
	puts ("cls             Clears Screen\n");
	puts ("color [XX]      Sets color [XX] = hex\n");
	puts ("echo [text]     Echos TEXT\n");
	puts ("help            Displays Help\n");
	puts ("ram             Outputs RAM info\n");
	puts ("exit            Exits CLI\n");
	puts ("------------DEBUG LIST---------------\n");
	puts ("inb [PORT]      Byte input PORT (HEX)\n");
	puts ("outb [PORT] [=] Byte output PORT = (HEX)\n");
	puts ("read X X X      Read Sector on X (X*X)\n");
	puts ("ramx X X        Read Ram X-X\n");
	
}

void do_CMD(int args)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 53) do_CMD(0x%x)\n\r", args);
#endif
	unsigned int i = 0;
	/*if (streql(explode_cmd[0], "initfat"))
		fsysFatInitialize ();*/
	if (streql(explode_cmd[0], "test")) {
		char *expOut[100];
		int MAX = 0;
		int z = 0;
		//const char *input = "testA 1234567890 12345678901234567890 123456789012345678901234567890 1234567890123456789012345678901234567890 12345678901234567890123456789012345678901234567890 123456789012345678901234567890123456789012345678901234567890 1234567890123456789012345678901234567890123456789012345678901234567890 12345678901234567890123456789012345678901234567890123456789012345678901234567890 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890 1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890 testB";
		const char *input = "/1/media/brian/6216059716056CFB/OS/CURRENT/PMODE/KERNEL/KERNEL.BIN";
		for(int tmz = 0;tmz < 10;tmz++) {
			MAX = explode(expOut, input, '/');
			for (z = 0; z < MAX; z++)
			{
			  printf("Value of TEST[%d] = \"%s\"\n", z, expOut[z] );
			}
			explodeFREE(expOut, MAX);
		}
	}
	else if (streql(explode_cmd[0], "cls"))
		cls();
	else if (streql(explode_cmd[0], "reboot"))
		outb(0x64, 0xFE);
	/*else if (streql(explode_cmd[0], "cat"))
		cat(explode_cmd[1]);*/
	else if (streql(explode_cmd[0], "clone"))
		CopyFloppyTest();
	else if (streql(explode_cmd[0], "exit"))
		CMD_ACTIVE = FALSE;
	else if (streql(explode_cmd[0], "help"))
		help();
	else if (streql(explode_cmd[0], "ram")) {
		printf("Total RAM:    %iKB\n", _mmngr_memory_size);
		printf("Free RAM:     %iKB\n", _mmngr_memory_size-(_mmngr_used_blocks*4));
		printf("Used RAM:     %iKB\n", _mmngr_used_blocks*4);
		txf(1, "Total RAM:    %iKB\n\r", _mmngr_memory_size);
		txf(1, "Free RAM:     %iKB\n\r", _mmngr_memory_size-(_mmngr_used_blocks*4));
		txf(1, "Used RAM:     %iKB\n\r", _mmngr_used_blocks*4);
	}
	else if (streql(explode_cmd[0], "echo")) {
		for (int e = 1; e < args; e++) {
			printf ("%s ", explode_cmd[e]);
			txf(1, "%s ", explode_cmd[e]);
		}
		txf(1, "\n\r");
		putch('\n');
	}
	else if (streql(explode_cmd[0], "color"))			{
		if( (i = textTOhex(explode_cmd[1])) != 0)
			setColor(i);								}
	else if (streql(explode_cmd[0], "inb")) {
		uint8_t in = inb((uint16_t) textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
		txf(1, "We recived %x = %i = %c\n\r", in, in, in);
	}
	else if (streql(explode_cmd[0], "outb")) {
		outb((uint16_t) textTOhex(explode_cmd[1]), (uint8_t) textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "inw")) {
		uint16_t in = inw((uint16_t) textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
		txf(1, "We recived %x = %i = %c\n\r", in, in, in);
	}
	else if (streql(explode_cmd[0], "outw")) {
		outw((uint16_t) textTOhex(explode_cmd[1]), (uint16_t) textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "inl")) {
		uint32_t in = inl((uint16_t) textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
		txf(1, "We recived %x = %i = %c\n\r", in, in, in);
	}
	else if (streql(explode_cmd[0], "outl")) {
		outl((uint16_t) textTOhex(explode_cmd[1]), (uint32_t) textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "ramx")) {
		uint64_t start = textTOhex(explode_cmd[1]);
		uint64_t end = textTOhex(explode_cmd[2]);
		for (uint64_t t = start; t <= end; t++) {
			printf("%x ", RAM[t]);
			txf(1, "%x ", RAM[t]);
		}
		txf(1, "\n\r");
		putch('\n');
	}
	else if (streql(explode_cmd[0], "read"))
		cmd_read((uint16_t) textTOhex(explode_cmd[1]), (uint64_t) textTOhex(explode_cmd[2]), (uint16_t) textTOhex(explode_cmd[3]));
	else if (streql(explode_cmd[0], "dir")) {
		if (args == 1)_VFS_DIR((const char*) CurrentPath);
		else _VFS_DIR((const char*) explode_cmd[1]);
	}
	else if (streql(explode_cmd[0], "user")) {
		enter_usermode();
		puts("Welcome to User Land!\n");
	}
	/*else if (streql(explode_cmd[0], "load")) {
		load(explode_cmd[1]);
	}*/
	else if (streql(explode_cmd[0], "kill"))
		KillThread((uint16_t) textTOhex(explode_cmd[1]));
	/*else if (streql(explode_cmd[0], "threadenable")) {
		_THREAD_MAN_enable();
		for(;;)
			putch('.');
	}*/
	else
		printf("Invalid Command \"%s\"\n", cmd_command);
}

void cmd_handler()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 115) cmd_handler()\n\r");
#endif
	memset(cmd_command, 0, sizeof cmd_command);
	printf("%s",CurrentPath);
	gets(cmd_command, "#");
	int args = explode (explode_cmd, cmd_command, ' ');
	do_CMD(args);
	explodeFREE(explode_cmd, args);
}

void init_cmd()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 128) init_cmd()\n\r");
#endif
	readbuffer = calloc(4);
	cmd_command = calloc(1);
	CurrentPath = (char *)"/";
	setColor (0x07);
	puts ("MyDOS v. 0.1\n");
	CMD_ACTIVE = TRUE;
}

void cmd_read(uint16_t partid, uint64_t start, uint16_t count)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 195) cmd_read(0x%x, 0x%x, 0x%x)\n\r", partid, start, count);
#endif
	if(count*0x200 > 0x4000)
		return;
	memset(readbuffer, 0, 0x4000);
	//! read sector from disk
	error errorcode;
	if((errorcode = Read_Partition(partid, readbuffer, start, count))) {
		printf("ERROR CODE = 0x%x\n", errorcode);
#ifdef DEBUG
	txf(1, "ERROR CODE = 0x%x\n\r", errorcode);
#endif
	}

	//! display sector
	if (readbuffer && !errorcode) {

		unsigned int i = 0;
		for ( unsigned int c = 0; c < count; c++ ) {
			printf ("\n\rSector %i contents:\n\r", (start + (uint64_t) (i/0x200)));
#ifdef DEBUG
	txf(1, "\n\rSector %i contents:\n\r", (start + (uint64_t) (i/0x200)));
#endif
			for (unsigned int j = 0; j < 0x200; j++)
				printf ("%x ", readbuffer[ i + j ]);
#ifdef DEBUG
	for (unsigned int j = 0; j < 0x200; j++)
		txf(1, "%x ", readbuffer[ i + j ]);
#endif
			i += 0x200;
		}

		printf ("\n\rDone.\n");
#ifdef DEBUG
	txf(1, "\n\rDone.\n\r");
#endif
	}
	else {
		printf ("\n\r*** Error reading sector from disk\n");
#ifdef DEBUG
	txf(1, "\n\r*** Error reading sector from disk\n\r");
#endif
	}
}

/*
void cat(const char* path)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 169) cat(%s)\n\r", path);
#endif
	//! open file
	FILE file = VFS_OpenFile (path);
	//! cant display directories
	if (( file.flags & FS_DIRECTORY )){// == FS_DIRECTORY) {
		printf ("\n\rUnable to display contents of directory.\n");
		return;
	}
	//! test for invalid file
	if (file.flags & FS_INVALID) {
		printf ("\n\rUnable to open file\n");
		return;
	}
	//! top line
	printf ("\n\r-------[%s]-------\n\r", file.name);
	//! display file contents
	while (file.eof != 1) {
		//! read cluster
		unsigned char buf[512];
		VFS_ReadFile ( &file, buf, 512);
		//! display file
		for (int i=0; i<512; i++)
			putch ((char)buf[i]);
		//! wait for input to continue if not EOF
		//if (file.eof != 1) {
			//char gethc = getch ("\n\r------[Press a key to continue]------");
			//if (gethc == 'q') return;
			//printf ("\r"); //clear last line
		//}
	}
	//! done :)
	printf ("\n\n\r--------[EOF]--------\n");
}

void load(const char* path)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 207) load(%s)\n\r", path);
#endif
	//! load file
	FILE file = VFS_OpenFile (path);
	//! test for invalid file
	if (file.flags == FS_INVALID) {
		printf ("\n\rUnable to open file\n");
		return;
	}
	//! cant load directories
	if (( file.flags & FS_DIRECTORY ) == FS_DIRECTORY) {
		printf ("\n\rUnable to display contents of directory.\n");
		return;
	}
	_THREAD_MAN_disable();
	uint16_t task = AddThread(0x800000, 0xFF, true);
	uint8_t *location = (uint8_t*) THREAD[task].physaddr;
	unsigned int x = 0;
	while (file.eof != 1) {
		//! read cluster
		unsigned char buf[512];
		VFS_ReadFile ( &file, buf, 512);
			memcpy (&location[x*0x200], buf, 0x200);
		x++;
	}
	THREAD[task].flags = 0x01;
	_THREAD_MAN_enable();
	//! done :)
	//for(uint32_t i = 0; i < 0xAAAAAAAA;i++) printf("%x\n", i);
}*/

void CopyFloppyTest()
{
	// I only want to test read / write seppeds so lets allocate memory now...
	uint8_t *FloppyTestBuffer = calloc(360);		//1=4KB so... 360=1.44M Be the biggist alloc test yet ;)
	uint32_t startTick, endTick;
	error errorcode = ERROR_NONE;
	//Sector By Sector Read
	startTick = GetTimerTicks();
	/*for(int i=0; i<80; i++)
		if((errorcode=_FDC_IO(false, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
			if((errorcode=_FDC_IO(false, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
				printf("FDC IO TRACK: 0x%x\tError: 0x%x\n", i, errorcode);*/


for(int i=0; i<12; i++)
		if((errorcode=_FDC_IO(false, (i * 240), 240, &FloppyTestBuffer[i*0x1E000])))
			if((errorcode=_FDC_IO(false, (i * 240), 240, &FloppyTestBuffer[i*0x1E000])))
				printf("FDC IO TRACK: 0x%x\tError: 0x%x\n", i, errorcode);


	endTick = GetTimerTicks();
	printf("Floppy TRACK Read Time = %i mS\n", (10 * (endTick - startTick)));
	(void) getch("Press Any Key To Write");
	putch('\n');
	//Sector By Sector Write
	startTick = GetTimerTicks();
	for(int i=0; i<80; i++)
		if((errorcode=_FDC_IO(true, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
			if((errorcode=_FDC_IO(true, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
				printf("FDC IO TRACK: 0x%x\tError: 0x%x\n", i, errorcode);
	endTick = GetTimerTicks();
	printf("Floppy TRACK Write Time = %i mS\n", (10 * (endTick - startTick)));
}
