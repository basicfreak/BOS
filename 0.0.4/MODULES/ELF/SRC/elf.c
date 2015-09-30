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
#include "ELF_INLINE.H"
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
void _LoadRelocElf(void* ELFLocation);
bool _RelocateExec(void* ELFPDir, void* ELFLocation);

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
	void* TempLocation = (void*) 0xC000;

	_API_INSTALL(NewBase, TempLocation, ((MyModList[0].ModEnd+1)-MyModList[0].ModStart));
	memcpy(TempLocation, 0x01000000, ((MyModList[0].ModEnd+1)-MyModList[0].ModStart));
	char* LELF = "LoadELF";
	char* FEELF = "FindEntryELF";
	_API_ADD((NewBase + (((uint32_t)&LoadELF) - 0x01000000)), (char*)((uint32_t)LELF + 0x01000000));
	_API_ADD((NewBase + (((uint32_t)&FindEntryELF) - 0x01000000)), (char*)((uint32_t)FEELF + 0x01000000));

	for(uint8_t mod = 1; mod < MyBoot->ModCount; mod++) {
		// DEBUG_printf("TESTING MOD #%i\n", mod);
		if(ValidELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart))) {
			// DEBUG_printf("Prepareing MOD #%i\n", mod);
			void* ELFPDir = LoadELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart));
			if(ELFPDir) {
				char *ModName = "COREMOD ";
				char *ModVirt = (char*) ((uint32_t)ModName + 0x01000000); // Fix the pointer
				ModVirt[7] = (char) (mod + 0x20);
				// DEBUG_printf("EXECUTEING MOD #%i\n", mod);
				_TM_EXEC(ELFPDir, FindEntryELF((void*)(ModsPhysicalOffset + MyModList[mod].ModStart)), ModVirt);
			}
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
		ELF32_Head_p Head = (ELF32_Head_p) ELFLocation;
		if(Head->e_type == ET_EXEC) {
			//EXEC
			_VMM_newDIR(ELFPDir);
			if(ELFPDir)
				_LoadExecElf(ELFPDir, ELFLocation);
			//
		} else if(Head->e_type == ET_REL) {
			//Relocatable
			_LoadRelocElf(ELFLocation);
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
		}
	}
	if(!_RelocateExec(ELFPDir, ELFLocation))
		for(;;);
}

void _LoadRelocElf(void* ELFLocation)
{
	ELF32_Head_p Head = (ELF32_Head_p) ELFLocation;

	// Calculate Actual Size of API Entry.
	uint32_t APISize = 0;
	for(uint16_t shd = 0; shd < ELF_SHead_Count(Head); shd++) {
		ELF32_SHead_p SHead = ELF_SHead(Head, shd);
		if(SHead->sh_flags & SHF_ALLOC) {
			APISize += (uint32_t)SHead->sh_size;
		}
	}
	uint32_t APILocation = 0;

	if(APISize <= 0xFF4000) {
		APILocation = 0xC000;
	} else if(APISize <= 0xF800000) {
		APILocation = 0xC0000000;
	} else {
		for(;;);
	}

	// Allocate API Space
	uint32_t APIBase = 0;
	_API_INSTALL(APIBase, APILocation, APISize);
	memset((void*)APILocation, 0, APISize);

	// Prepare Sections.
	uint32_t APIOffset = 0;
	for(uint16_t shd = 0; shd < ELF_SHead_Count(Head); shd++) {
		ELF32_SHead_p SHead = ELF_SHead(Head, shd);
		if(SHead->sh_flags & SHF_ALLOC) {
			// Copy Data Here...
			if(SHead->sh_size) {
				if(!(SHead->sh_type & SHT_NOBITS))
					memcpy((APILocation + APIOffset), ((uint32_t)ELFLocation + SHead->sh_offset), (SHead->sh_size));
				SHead->sh_addr = APIBase + APIOffset;
				APIOffset += (uint32_t)SHead->sh_size;
			}
		}
	}

	// Relocate Sections.
	for(uint16_t shd = 0; shd < ELF_SHead_Count(Head); shd++) {
		ELF32_SHead_p SHead = ELF_SHead(Head, shd);
		if(SHead->sh_type == SHT_REL) {
			ELF32_SHead_p target = ELF_SHead(ELFLocation, SHead->sh_info);
			for(uint32_t x = 0; x < (uint32_t)(SHead->sh_size / SHead->sh_entsize); x++) {
				ELF32_Rel_p RelocTable = (ELF32_Rel_p) (((uint32_t) ELFLocation + (uint32_t)SHead->sh_offset) + (uint32_t) (x * (uint32_t)SHead->sh_entsize));
				int addr = (int)target->sh_addr;
				int *ref = (int *)(addr + (int)RelocTable->r_offset);
				int symval = 0;
				if(ELF32_R_SYM(RelocTable->r_info) != 0) {
					symval = ELF_Get_SymVal(ELFLocation, SHead->sh_link, ELF32_R_SYM(RelocTable->r_info));
					if(symval == 0) {
						for(;;);
					}
					int *vref = (int*)(APILocation + ((uint32_t)ref - APIBase));
					switch(ELF32_R_TYPE(RelocTable->r_info)) {
						case R_386_NONE:
							// No relocation
							break;
						case R_386_32:
							*vref = DO_386_32(symval, *vref);
							break;
						case R_386_PC32:
							*vref = DO_386_PC32(symval, *vref, (int)ref);
							break;
						default:
							for(;;);
					}
				}
			}
		}
	}

	// Search For Symbols.
	for(uint16_t shd = 0; shd < ELF_SHead_Count(Head); shd++) {
		ELF32_SHead_p SHead = ELF_SHead(Head, shd);
		if(SHead->sh_type == SHT_SYMTAB) {
			for(uint32_t x = 0; x < ((uint32_t)SHead->sh_size); x+=((uint32_t)SHead->sh_entsize)) {
				ELF32_SymTbl_p STab = (ELF32_SymTbl_p) ((uint32_t)ELFLocation + SHead->sh_offset + x);
				if((STab->st_info & STT_FUNC) && STab->st_name) {
					char* SymName = ELF_Get_String(Head, shd, STab->st_name);
					if(SymName[0] != '_') // Leading underscore means private function.
						_API_ADD((( ELF_SHead(Head, (STab->st_info >> 4))->sh_addr ) + STab->st_value), SymName);
				}
			}
		}
	}
}

bool _RelocateExec(void* ELFPDir, void* ELFLocation)
{
	ELF32_Head_p Head = (ELF32_Head_p) ELFLocation;
	for(int shnum = 0; shnum < Head->e_shnum; shnum++) {
		ELF32_SHead_p reltab = (ELF32_SHead_p) ((uint32_t)ELFLocation + (uint32_t)Head->e_shoff + ((uint32_t)Head->e_shentsize * shnum));
		if(reltab->sh_type == SHT_REL) {
			// Do Reloc
			ELF32_SHead_p target = ELF_SHead(ELFLocation, reltab->sh_info);
			for(uint32_t x = 0; x < (uint32_t)(reltab->sh_size / reltab->sh_entsize); x++) {
				ELF32_Rel_p RelocTable = (ELF32_Rel_p) (((uint32_t) ELFLocation + (uint32_t)reltab->sh_offset) + (uint32_t) (x * (uint32_t)reltab->sh_entsize));
				// int addr = (int)target->sh_addr;// + (int)target->sh_offset;
				if(ELF_IsSymUNDEF(ELFLocation, reltab->sh_link, ELF32_R_SYM(RelocTable->r_info))) {
					int *ref = (int *)(RelocTable->r_offset); // ELF SPEC 1-12 r_offset EXEC "virtual address of the storage unit affected by the relocation."
					uint32_t PhysOfOther = 0;
					_VMM_getPageOther(ELFPDir, ((uint32_t)ref & 0xFFFFF000), PhysOfOther);
					_VMM_map (0xC000, PhysOfOther, TRUE);
					// Symbol value
					int symval = 0;
					if(ELF32_R_SYM(RelocTable->r_info) != 0) {
						symval = ELF_Get_SymVal(ELFLocation, reltab->sh_link, ELF32_R_SYM(RelocTable->r_info));
						if(symval == 0) {
							return FALSE;
						}
						int* vref = (int*)(0xC000 + ((uint32_t)ref & 0x0FFF));
						switch(ELF32_R_TYPE(RelocTable->r_info)) {
							case R_386_NONE:
								// No relocation
								break;
							case R_386_32:
								*vref = DO_386_32(symval, *vref);
								break;
							case R_386_PC32:
								*vref = DO_386_PC32(symval, *vref, (uint32_t)0);
								break;
							default:
								return FALSE;
						}
					}
				}
			}
			_VMM_umap (0xC000);
		}
	}
	return TRUE;
}

void memset(void *dest, uint8_t val, uint32_t count)
{
	uint8_t *temp = (uint8_t *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
}
