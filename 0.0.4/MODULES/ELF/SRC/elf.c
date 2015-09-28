/*
NOTE THIS IS A VERY DELICATE FILE!
This is part of a shared binary.
We are linked to 0x00000000
First Executed at 0x01000000
Then moved into ABI anywhere from 0xE4200000 to 0xFF000000
(probably closer to 0xE4200000, as this is the first app)
*/


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

void memset(void *dest, uint8_t val, uint32_t count);
bool ValidELF(void* ELFLocation);
void* LoadELF(void* ELFLocation);
void* FindEntryELF(void* ELFLocation);
void _LoadExecElf(void *ELFPDir, void *ELFLocation);
void _LoadRelocElf(void* ELFPDir, void* ELFLocation);

// extern void outb(uint16_t, uint8_t);

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
	// We also know we are located at 0x01000000 (for now)
	// So find the difference between 0x01000000 and MyModList[0].ModStart
	uint32_t ModsPhysicalOffset = (0x01000000 - MyModList[0].ModStart);

	// We need to load this mod into API so it is accessable to load ELF at any time.
	uint32_t NewBase = 0;
	void* TempLocation = (void*) 0xF0000;

	_API_INSTALL(NewBase, TempLocation, ((MyModList[0].ModEnd+1)-MyModList[0].ModStart));
	memcpy(TempLocation, 0x01000000, ((MyModList[0].ModEnd+1)-MyModList[0].ModStart));
	_API_ADD((NewBase + (((uint32_t)&LoadELF) - 0x01000000)), "LoadELF");

	for(uint8_t mod = 1; mod < MyBoot->ModCount; mod++) {
		// DEBUG_printf("TESTING MOD #%i\n", mod);
		if(ValidELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart))) {
			// DEBUG_printf("Prepareing MOD #%i\n", mod);
			void* ELFPDir = LoadELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart));
			char *ModName = "COREMOD ";
			//char *ModVirt = (char*) ((uint32_t)ModName + 0x01000000); // Fix the pointer
			ModName[7] = (char) (mod + 0x20);
			// DEBUG_printf("EXECUTEING MOD #%i\n", mod);
			_TM_EXEC(ELFPDir, FindEntryELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart)), ModName);
		}
	}
	uint32_t PhysMemEnd = (MyModList[(MyBoot->ModCount - 1)].ModEnd + 1);
	if(PhysMemEnd % 0x1000)
		PhysMemEnd = (PhysMemEnd + (0x1000 - (PhysMemEnd % 0x1000)));
	for(uint32_t VirtMem = (ModsPhysicalOffset + MyModList[1].ModStart); VirtMem < (ModsPhysicalOffset + PhysMemEnd); VirtMem += 0x1000)
		_VMM_umapFree(VirtMem);

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
	if(ValidELF(ELFLocation)) {
		_VMM_newDIR(ELFPDir);
		if(ELFPDir) {
			ELF32_Head_p Head = (ELF32_Head_p) ELFLocation;
			if(Head->e_type == ET_EXEC) {
				//EXEC
				_LoadExecElf(ELFPDir, ELFLocation);
				//
			} else if(Head->e_type == ET_REL) {
				//Relocatable
				_LoadRelocElf(ELFPDir, ELFLocation);
			} else {
				//UNKNOWN
				return 0;
			}
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
		// DEBUG_printf("Header #%i\n", Header);
		if(PHead[Header].p_memsz) { // If there is any memory here
			void* startAddress = (void*) PHead[Header].p_vaddr;
			uint32_t Pages = (PHead[Header].p_memsz / 0x1000);
			if(PHead[Header].p_memsz % 0x1000)
				Pages++;
			// DEBUG_printf("Virtual Pages #%i\n", Pages);
			for(uint32_t page = 0; page < Pages; page++) {
				// DEBUG_printf("ALLOCATE\n");
				_VMM_allocOther((uint32_t)ELFPDir, ((uint32_t)startAddress + (page * 0x1000)), TRUE);
				uint32_t physicalPage = 0;
				// DEBUG_printf("GetPhys\n");
				_VMM_getPageOther((uint32_t)ELFPDir, ((uint32_t)startAddress + (page * 0x1000)), physicalPage);
				// DEBUG_printf("MAP\n");
				_VMM_map(0xC000, (physicalPage), TRUE);
				if((((int)PHead[Header].p_filesz) - (int)(page * 0x1000)) >= 0x1000) {
					memcpy(0xC000, (((uint32_t)ELFLocation + PHead[Header].p_offset) + (page * 0x1000)), 0x1000);
				} else if((((int)PHead[Header].p_filesz) - (int)(page * 0x1000)) > 0) {
					memcpy(0xC000, (((uint32_t)ELFLocation + PHead[Header].p_offset) + (page * 0x1000)), ((PHead[Header].p_filesz) - (page * 0x1000)));
					memset((0xC000 + ((PHead[Header].p_filesz) - (page * 0x1000))), 0, (0x1000 - ((PHead[Header].p_filesz) - (page * 0x1000))));
				} else {
					memset(0xC000, 0, 0x1000);
				}
			}
			_VMM_umap(0xC000);
			// DEBUG_printf("COPY DATA\n");
			// memcpy((0x81000000), ((uint32_t)ELFLocation + PHead[Header].p_offset), (PHead[Header].p_filesz));
			// DEBUG_printf("Zero REMANDER\n");
			// if(PHead[Header].p_memsz > PHead[Header].p_filesz)
				// memset((void*)(0x81000000 + PHead[Header].p_filesz), 0, (uint32_t)(PHead[Header].p_memsz - PHead[Header].p_memsz));
			// DEBUG_printf("UMAP\n");
			// for(uint32_t page = 0; page < Pages; page++) {
				// _VMM_umap((0x81000000 + (page * 0x1000)));
			// }
		}
	}
}

void _LoadRelocElf(void* ELFPDir, void* ELFLocation)
{
	//
}

void memset(void *dest, uint8_t val, uint32_t count)
{
	uint8_t *temp = (uint8_t *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
}
