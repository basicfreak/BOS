#include <typedefines.h>
#include <BOS/BOSBOOT.h>
#include <systemcalls.h>
#include <io.h>
#include "ELF_DATA.H"
/*
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/
int main(BootInfo_p BOOTINF);

#define DEBUG_putch(x) outb(0xE9, x)

void DEBUG_print(const char* Str);
// void DEBUG_putch(const char Chr);
void DEBUG_printf(const char* Str, ...);
void itoa(uint32_t i, uint32_t base, uint8_t* buf);

void memset(void *dest, uint8_t val, uint32_t count);
bool ValidELF(void* ELFLocation);
void* LoadELF(void* ELFLocation);
void* FindEntryELF(void* ELFLocation);
void _LoadExecElf(void *ELFPDir, void *ELFLocation);

// extern void outb(uint16_t, uint8_t);

void NewThreadAA(void);

int main(BootInfo_p BOOTINF)
{
	_IOPERM(); // Request I/O Permission

	// __asm__ __volatile__ ( "int $0xF2" : : "a" (0x80), "d" (0x10000), "b" (1), "c" (((uint32_t)BOOTINF) & 0xFFFFF000));

	_VMM_map(0x10000, (((uint32_t)BOOTINF) & 0xFFFFF000), FALSE);

	BootInfo_p MyBoot = (BootInfo_p) (0x10000 + ((uint32_t)BOOTINF % 0x1000));
	ModL_p MyModList;
	if(((uint32_t)MyBoot->ModList & 0xFFFFF000) == ((uint32_t)BOOTINF & 0xFFFFF000))
		MyModList = (ModL_p) (0x10000 + ((uint32_t)MyBoot->ModList % 0x1000));
	else {
		_VMM_map(0x11000, ((uint32_t)MyBoot->ModList & 0xFFFFF000), FALSE);
		MyModList = (ModL_p) (0x11000 + ((uint32_t)MyBoot->ModList % 0x1000));
	}

	// We know we are the first mod.
	// We also know we are located at 0x01000000
	// So find the difference between 0x01000000 and MyModList[0].ModStart
	uint32_t ModsPhysicalOffset = (0x01000000 - MyModList[0].ModStart);

	for(uint8_t mod = 1; mod < MyBoot->ModCount; mod++) {
		DEBUG_printf("TESTING MOD #%i\n", mod);
		if(ValidELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart))) {
			DEBUG_printf("Prepareing MOD #%i\n", mod);
			void* ELFPDir = LoadELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart));
			char *ModName = "COREMOD ";
			ModName[7] = (char) (mod + 0x20);
			DEBUG_printf("EXECUTEING MOD #%i\n", mod);
			_TM_EXEC(ELFPDir, FindEntryELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart)), ModName);
		}
	}

	return TRUE;
}

bool ValidELF(void* ELFLocation) // NOTE DOES NOT CHECK EVERYTHING
{
	ELF32_Head_p Head = (ELF32_Head_p) ELFLocation;
	if(Head->e_ident.ei_mag1 == 'E')
		if(Head->e_ident.ei_mag2 == 'L')
			if(Head->e_ident.ei_mag3 == 'F')
				if(Head->e_version == EV_CURRENT)
					if(Head->e_machine == EM_386)
						if(Head->e_ident.ei_class == ELFCLASS32)
							return TRUE;
	return FALSE;
}

void* LoadELF(void* ELFLocation)
{
	void* ELFPDir = 0;
	_VMM_newDIR(ELFPDir);
	if(ELFPDir) {
		ELF32_Head_p Head = (ELF32_Head_p) ELFLocation;
		if(Head->e_type == ET_EXEC) {
			//EXEC
			_LoadExecElf(ELFPDir, ELFLocation);
			//
		} else if(Head->e_type == ET_REL) {
			//Relocatable
		} else {
			//UNKNOWN
			return 0;
		}
	}
	return ELFPDir;
}

void* FindEntryELF(void* ELFLocation)
{
	return (void*)(((ELF32_Head_p) ELFLocation)->e_entry);
}


void _LoadExecElf(void *ELFPDir, void *ELFLocation)
{
	ELF32_Head_p Head = (ELF32_Head_p) ELFLocation;
	ELF32_PHead_p PHead = (ELF32_PHead_p) ((uint32_t)ELFLocation + Head->e_phoff);

	for(uint16_t Header = 0; Header < Head->e_phnum; Header++) {
		DEBUG_printf("Header #%i\n", Header);
		if(PHead[Header].p_memsz) { // If there is any memory here
			void* startAddress = (void*) PHead[Header].p_vaddr;
			uint32_t Pages = (PHead[Header].p_memsz / 0x1000);
			if(PHead[Header].p_memsz % 0x1000)
				Pages++;
			DEBUG_printf("Virtual Pages #%i\n", Pages);
			for(uint32_t page = 0; page < Pages; page++) {
				DEBUG_printf("ALLOCATE\n");
				 _VMM_allocOther(ELFPDir, ((uint32_t)startAddress + (page * 0x1000)), TRUE);
				 void* physicalPage = 0;
				 DEBUG_printf("GetPhys\n");
				 _VMM_getPageOther(ELFPDir, ((uint32_t)startAddress + (page * 0x1000)), physicalPage);
				 DEBUG_printf("MAP\n");
				 _VMM_map((void*)(0x81000000 + (page * 0x1000)), (physicalPage), TRUE);
			}
			DEBUG_printf("COPY DATA\n");
			memcpy((void*)(0x81000000), ((uint32_t)ELFLocation + PHead[Header].p_offset), (PHead[Header].p_filesz));
			DEBUG_printf("Zero REMANDER\n");
			if(PHead[Header].p_memsz > PHead[Header].p_filesz)
				memset((void*)(0x81000000 + PHead[Header].p_filesz), 0, (uint32_t)(PHead[Header].p_memsz - PHead[Header].p_memsz));
			DEBUG_printf("UMAP\n");
			for(uint32_t page = 0; page < Pages; page++) {
				_VMM_umap((0x81000000 + (page * 0x1000)));
			}
		}
	}
}

void memset(void *dest, uint8_t val, uint32_t count)
{
	uint8_t *temp = (uint8_t *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
}

















void DEBUG_print(const char* Str)
{
	while(*Str)
		DEBUG_putch(*Str++);
}

// void DEBUG_putch(const char Chr)
// {
// 	outb(0xE9, (uint8_t) Chr);
// }

void DEBUG_printf(const char* Str, ...)
{
	va_list ap;
	va_start(ap, Str);
	while(*Str) {
		if(*Str == '%') {
			*Str++;
			switch(*Str) {
				/*** characters ***/
				case 'c': {
					const char c = (const char) va_arg (ap, const char);
					DEBUG_putch(c);
					*Str++;
					break;
				}
				/*** integers ***/
				case 'd':
				case 'i':
				{
					uint32_t c = va_arg (ap, uint32_t);
					uint8_t s[32]={0};
					itoa (c, 10, s);
					DEBUG_print((const char*) s);
					*Str++;		// go to next character
					break;
				}
				/*** display in hex ***/
				case 'X':
				case 'x':
				{
					uint32_t c = va_arg (ap, uint32_t);
					uint8_t s[32]={0};
					itoa (c,16,s);
					DEBUG_print((const char*) s);
					*Str++;		// go to next character
					break;
				}
				/*** strings ***/
				case 's':
				{
					const char *s = va_arg (ap, const char*);
					DEBUG_print((const char*) s);
					*Str++;		// go to next character
					break;
				}
				case '%':
				{
					DEBUG_putch('%');
					*Str++;
					break;
				}
			}
		} else {
			DEBUG_putch(*Str++);
		}
	}
}
uint8_t		tbuf[32];
uint8_t		bchars[] =				{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(uint32_t i, uint32_t base, uint8_t* buf)
{
   uint32_t pos = 0;
   uint32_t opos = 0;
   uint32_t top = 0;

   if (i == 0 || base > 16)
   {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0)
   {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++)
   {
      buf[opos] = tbuf[pos];
   }
   buf[opos] = 0;
   return;
}