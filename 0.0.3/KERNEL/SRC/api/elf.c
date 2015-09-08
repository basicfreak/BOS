/*
./SRC/ELF/ELF.C
*/

#include <api/elf.h>
#include "ELF_DATA.H"
#include <i386/memory/heapman.h>
#include <api/api.h>

void (*_INIT_)(uint32_t) = 0;

// Declare Local Functions:
uint32_t ELF_Hash(const unsigned char *name);
void _ELF_PREPARE_SECTIONS(void *File);
bool elf_do_reloc(void *File, uint16_t relocSection);
#ifdef DEBUG_FULL
	static inline const char *SHead_Type_Decode(uint32_t type);
#endif

// Local Functions:
uint32_t ELF_Hash(const unsigned char *name)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t h = 0, g;
	while (*name)
	{
		h = ((h << 4) + *name++);
		if ((g = (h & 0xF0000000)))
			h ^= (g >> 24);
		h &= ~g;
	}
	return h;
}

#ifdef DEBUG_FULL
	static inline const char *SHead_Type_Decode(uint32_t type) {
		return (const char *) ( (type == SHT_NULL) ? "Null Section" :
				(type == SHT_PROGBITS) ? "Program Information" :
				  (type == SHT_SYMTAB) ? "Symbol Table" :
				    (type == SHT_STRTAB) ? "String Table" :
				      (type == SHT_RELA) ? "Relocation (With Addend)" :
				        (type == SHT_NOBITS) ? "Not Present In File" :
				          (type == SHT_REL) ? "Relocation (No Addend)" : "UNKNOWN TYPE" );
	}
#endif

void _ELF_PREPARE_SECTIONS(void *File)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	ELF32_Head_p Head = File;
	for(uint16_t shd = 0; shd < ELF_SHead_Count(Head); shd++) {
		ELF32_SHead_p SHead = ELF_SHead(Head, shd);
		if((SHead->sh_type == SHT_NOBITS) && (SHead->sh_flags & SHF_ALLOC)) {
			// DEBUG_printf("\n\t\tRelocated Section %i\n", shd);
			// uint32_t pages = ((SHead->sh_size / 0x1000) + ((SHead->sh_size % 0x1000) ? 1 : 0));
			// if(!pages)
			// 	pages++;
			// void *SectionLoc = calloc_page(pages);
			void *SectionLoc = calloc(SHead->sh_size);
			SHead->sh_offset = (Elf32_Off) ((Elf32_Off)SectionLoc - (Elf32_Off) File);
			//SHead->sh_addr = (Elf32_Addr) SectionLoc;
		}
	}
}

bool elf_do_reloc(void *File, uint16_t relocSection)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif

// DEBUG_printf("DO_RELOC...");
// ELF_Display_Data(File);
	ELF32_Head_p hdr = File;
	ELF32_SHead_p reltab = ELF_SHead(hdr, relocSection);
	ELF32_SHead_p target = ELF_SHead(hdr, reltab->sh_info);
 	//volatile ELF32_Rel_p RelocTable;
/*DEBUG_printf("\n\t\t\t\tFILE       \t0x%x\n", (uint32_t)File);
DEBUG_printf("%i\t\t\t\tOFFSET     \t0x%x\n", relocSection, (uint32_t)reltab->sh_offset);*/
	for(uint32_t x = 0; x < (uint32_t)(reltab->sh_size / reltab->sh_entsize); x++) {
		ELF32_Rel_p RelocTable = (ELF32_Rel_p) (((uint32_t) File + (uint32_t)reltab->sh_offset) + (uint32_t) (x * (uint32_t)reltab->sh_entsize));
/*		DEBUG_printf("%i\t\t\t\trel        \t0x%x\n", x, (uint32_t)RelocTable);
		DEBUG_printf("%i\t\t\t\tOffset     \t0x%x\n", x, RelocTable->r_offset);
		DEBUG_printf("%i\t\t\t\tInfo       \t0x%x\n", x, RelocTable->r_info);
		DEBUG_printf("%i\t\t\t\tInfo_SYM   \t0x%x\n", x, ELF32_R_SYM(RelocTable->r_info));
		DEBUG_printf("%i\t\t\t\tInfo_TYPE  \t0x%x\n", x, ELF32_R_TYPE(RelocTable->r_info));*/
		int addr = (int)hdr + (int)target->sh_offset;
		int *ref = (int *)(addr + (int)RelocTable->r_offset);
		// Symbol value
		int symval = 0;
		if(ELF32_R_SYM(RelocTable->r_info) != 0) {
			//symval = elf_get_symval(hdr, reltab->sh_link, ELF32_R_SYM(RelocTable->r_info));
// DEBUG_printf("0x%x, 0x%x, 0x%x\n", hdr, reltab->sh_link, ELF32_R_SYM(RelocTable->r_info));
// DEBUG_printf("0x%x = GETVALUE(0x%x, 0x%x, 0x%x)...", ELF_Get_SymVal(hdr, reltab->sh_link, ELF32_R_SYM(RelocTable->r_info)), hdr, reltab->sh_link, ELF32_R_SYM(RelocTable->r_info));
			symval = ELF_Get_SymVal(hdr, reltab->sh_link, ELF32_R_SYM(RelocTable->r_info));
			if(symval == 0) {
#ifdef DEBUG
	DEBUG_printf("\nERROR Linking ELF\n");
#endif
				return FALSE;
			}
		}
		// Relocate based on type
		// DEBUG_printf("\n\t\t%i\n", (ELF32_R_TYPE(RelocTable->r_info)));
		switch(ELF32_R_TYPE(RelocTable->r_info)) {
			case R_386_NONE:
			// DEBUG_printf("\n\t\nNONE\n");
// DEBUG_printf("NO_RELOC...");
				// No relocation
				break;
			case R_386_32:
				// Symbol + Offset
			// DEBUG_printf("\n\t\tALPHA\n");
			// DEBUG_printf("\tDO_386_32(0x%x, 0x%x) = 0x%x\n", (uint32_t) symval, (uint32_t) *ref, (uint32_t)(DO_386_32(symval, *ref)));
// DEBUG_printf("386_32_RELOC (0x%x = (0x%x, 0x%x)...", DO_386_32(symval, *ref), symval, *ref);
				*ref = DO_386_32(symval, *ref);
				break;
			case R_386_PC32:
// DEBUG_printf("386_PC32_RELOC.(0x%x = (0x%x, 0x%x, 0x%x)...", DO_386_PC32(symval, *ref, (int)ref), symval, *ref, (int)ref);
				// Symbol + Offset - Section Offset
			// DEBUG_printf("\n\t\tBETA\n");
			// DEBUG_printf("\tDO_386_PC32(0x%x, 0x%x, 0x%x) = 0x%x\n", (uint32_t)symval, (uint32_t)*ref, (uint32_t)target, (uint32_t)(DO_386_PC32(symval, *ref, (int)target)));
				*ref = DO_386_PC32(symval, *ref, (int)ref);//((int)target - 6503));
				break;
			default:
			// DEBUG_printf("\n\t\nNONE2\n");
				return FALSE;
		}
	}
	return TRUE;
}


// Global Functions:
uint32_t Load_ELF(void *File)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
// DEBUG_printf("LOAD_ELF...");
	ELF32_Head_p Head = File;
	if(!ELF_isSigned(Head))
		return FALSE;
// DEBUG_printf("PREPARE_ELF...");
	_ELF_PREPARE_SECTIONS(File);
	if(ELF_isRelocatable(Head)) {
// DEBUG_printf("RELOCATE_ELF...");
		// Relocatable ELF
		for(uint16_t shd = 0; shd < (uint16_t) ELF_SHead_Count(Head); shd++)
			if(ELF_SHead(Head, shd)->sh_type == SHT_REL)
				if(!elf_do_reloc(File, shd))
					return FALSE;
	} else if(ELF_isExecutable(Head)) {
		// Non-Relocatable ELF
		return FALSE;
	} else
		return FALSE;
// DEBUG_printf("RETURN_ENTRY (0x%x)...", ELF_Get_Entry(File));
	return ELF_Get_Entry(File);
}

#ifdef DEBUG
	void ELF_Display_Data(void *File)
	{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
		ELF32_Head_p Head = File;
		if(ELF_isSigned(Head)) {
			DEBUG_printf("Valid ELF Signiture Located!\n");
			DEBUG_printf( ( (ELF_is32BIT(Head)) ? "\tELF is \"32-BIT\"\n" :
					"\tELF is NOT!! \"32-BIT\"!\n" ) );
			DEBUG_printf( ( (ELF_isLittleEndian(Head)) ? "\tELF is \"Little-Endian Formatted\"\n" :
					"\tELF is NOT!! \"Little-Endian Formatted\"!\n" ) );
			DEBUG_printf( ( (ELF_isRelocatable(Head)) ? "\tELF Type is \"Relocatable\"\n" :
					( (ELF_isExecutable(Head)) ? "\tELF Type is \"Executable\"\n" :
					  ( (ELF_isSharedObject(Head)) ? "\tELF Type is \"Shared Object\"\n" :
						"\tELF Type is \"UNKNOWN\"! (0x%x | %i)\n" ) ) ), Head->e_type, Head->e_type);

			DEBUG_printf( ( (ELF_is386(Head)) ? "\tELF is for \"i386+\"\n" :
					"\tELF is NOT!! for \"i386+\"!\n" ) );
			DEBUG_printf( ( (ELF_isCurrent(Head)) ? "\tELF Version is Current\n" :
					"\tELF Version is NOT!! Current!\n" ) );

			DEBUG_printf("\tELF has %i Section Headers\n", ELF_SHead_Count(Head));
			DEBUG_printf("\tELF Section Headers Base = 0x%x\n", ELF_SHead_Base(Head));
			for(uint16_t shd = 0; shd < ELF_SHead_Count(Head); shd++) {
				ELF32_SHead_p SHead = ELF_SHead(Head, shd);
				DEBUG_printf("\tSection Header %i\tHeader  Location  0x%x\n", shd, (uint32_t) SHead);
				DEBUG_printf("\t                 \tSection Location  0x%x:\n", (uint32_t) ELF_Section(Head, shd));
#ifdef DEBUG_FULL
				DEBUG_printf("\t\tType       \t0x%x\t\"%s\"\n", SHead->sh_type,
						SHead_Type_Decode(SHead->sh_type));
#else
				DEBUG_printf("\t\tType       \t0x%x\n", SHead->sh_type);
#endif
				//DEBUG_printf("\t\tName  (%i)\n", SHead->sh_name);
				DEBUG_printf("\t\tName  (%i)\t\"%s\"\n", SHead->sh_name, ( (SHead->sh_name) ?
						(ELF_SHead_Get_Name(Head, shd)) : "No Name!" ) );
				DEBUG_printf("\t\tFlags      \t0x%x\n", SHead->sh_flags);
				DEBUG_printf("\t\tAddress    \t0x%x\n", SHead->sh_addr);
				DEBUG_printf("\t\tOffset     \t0x%x\n", SHead->sh_offset);
				DEBUG_printf("\t\tSize       \t0x%x\n", SHead->sh_size);
				DEBUG_printf("\t\tLink       \t0x%x\n", SHead->sh_link);
				DEBUG_printf("\t\tInfo       \t0x%x\n", SHead->sh_info);
				DEBUG_printf("\t\tAddrAlign  \t0x%x\n", SHead->sh_addralign);
				DEBUG_printf("\t\tEntSize    \t0x%x\n", SHead->sh_entsize);

				if(SHead->sh_type == SHT_SYMTAB) {
					DEBUG_printf("\t\t\tSYMBOL TABLE:\n");
					volatile ELF32_SymTbl_p SymTbl;
					for(unsigned Sym = 0; Sym < (SHead->sh_size / 16); Sym++) {
						SymTbl = (ELF32_SymTbl_p) (((uint32_t) ELF_Section(Head, shd)) + (Sym * SHead->sh_entsize));
						if(SymTbl->st_name) {
							DEBUG_printf("\t\t\tName  (%i)\t\t\"%s\"\n", SymTbl->st_name,
									( (SymTbl->st_name) ? (ELF_Get_String(Head, shd, SymTbl->st_name)) :
									  ("No Name!") ));
							// DEBUG_printf("\t\t\tName  (%i)\n", SymTbl->st_name);
							DEBUG_printf("\t\t\t\tValue      \t0x%x\n", SymTbl->st_value);
							DEBUG_printf("\t\t\t\tSize       \t0x%x\n", SymTbl->st_size);
							DEBUG_printf("\t\t\t\tInfo       \t0x%x\n", SymTbl->st_info);
							DEBUG_printf("\t\t\t\tOther      \t0x%x\n", SymTbl->st_other);
							DEBUG_printf("\t\t\t\tShndx      \t0x%x\n", SymTbl->st_shndx);
						}
					}
				}
				if(SHead->sh_type == SHT_REL) {
					DEBUG_printf("\t\t\tRELOC TABLE:\n");
					volatile ELF32_Rel_p RelocTable;
					for(int x = 0; x < (SHead->sh_size / SHead->sh_entsize); x++) {
						RelocTable = (ELF32_Rel_p) ((uint32_t) ELF_Section(Head, shd) + (uint32_t) (x * SHead->sh_entsize));
						DEBUG_printf("\n%i\t\t\t\trel        \t0x%x\n", x, (uint32_t)RelocTable);
						DEBUG_printf("%i\t\t\t\tOffset     \t0x%x\n", x, RelocTable->r_offset);
						DEBUG_printf("%i\t\t\t\tInfo       \t0x%x\n", x, RelocTable->r_info);
						DEBUG_printf("%i\t\t\t\tInfo_SYM   \t0x%x\n", x, ELF32_R_SYM(RelocTable->r_info));
						DEBUG_printf("%i\t\t\t\tInfo_TYPE  \t0x%x\n", x, ELF32_R_TYPE(RelocTable->r_info));
					}
				}
			}




#ifdef DEBUG	
			DEBUG_printf("\n\n\t\tSigniture  \t0x%x \"%c%c%c\"\n", Head->e_ident.ei_mag0,
					 Head->e_ident.ei_mag1, Head->e_ident.ei_mag2, Head->e_ident.ei_mag3);
			DEBUG_printf("\t\tClass      \t0x%x\n", Head->e_ident.ei_class);
			DEBUG_printf("\t\tData       \t0x%x\n", Head->e_ident.ei_data);
			DEBUG_printf("\t\tVersion    \t0x%x\n", Head->e_ident.ei_version);
			DEBUG_printf("\t\tOSABI      \t0x%x\n", Head->e_ident.ei_osabi);
			DEBUG_printf("\t\tABIVersion \t0x%x\n", Head->e_ident.ei_abiversion);
			DEBUG_printf("\t\tPad        \t0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Head->e_ident.ei_pad[0],
					 Head->e_ident.ei_pad[1], Head->e_ident.ei_pad[2], Head->e_ident.ei_pad[3], Head->e_ident.ei_pad[4],
					  Head->e_ident.ei_pad[5], Head->e_ident.ei_pad[6]);
			DEBUG_printf("\t\te_type     \t0x%x\n", Head->e_type);
			DEBUG_printf("\t\te_machine  \t0x%x\n", Head->e_machine);
			DEBUG_printf("\t\te_version  \t0x%x\n", Head->e_version);
			DEBUG_printf("\t\te_entry    \t0x%x\n", Head->e_entry);
			DEBUG_printf("\t\te_phoff    \t0x%x\n", Head->e_phoff);
			DEBUG_printf("\t\te_shoff    \t0x%x\n", Head->e_shoff);
			DEBUG_printf("\t\te_flags    \t0x%x\n", Head->e_flags);
			DEBUG_printf("\t\te_ehsize   \t0x%x\n", Head->e_ehsize);
			DEBUG_printf("\t\te_phentsize\t0x%x\n", Head->e_phentsize);
			DEBUG_printf("\t\te_phnum    \t0x%x\n", Head->e_phnum);
			DEBUG_printf("\t\te_shentsize\t0x%x\n", Head->e_shentsize);
			DEBUG_printf("\t\te_shnum    \t0x%x\n", Head->e_shnum);
			DEBUG_printf("\t\te_shstrndx \t0x%x\n", Head->e_shstrndx);
#endif
			/*if(Load_ELF(File)) {
				//_INIT_ = (void (*)(void)) ((uint32_t) File + 0x1040);
				_INIT_ = (void (*)(uint32_t)) ELF_Get_Entry(File);
				DEBUG_printf("_INIT_ = 0x%x\n", (uint32_t) _INIT_);
				_INIT_((uint32_t)&_API_Get_Symbol);
				DEBUG_printf("\nDone Executing\n");


void (*puts)(const char* STR) = (void(*)(const char*)) _API_Get_Symbol("puts");
void (*putch)(const char charactor) = (void(*)(const char)) _API_Get_Symbol("putch");

	putch('.');
	puts("\tKernel Using First Module Ever!\n");

			} else
				DEBUG_printf("ERROR LOADING ELF\n");*/
		} else
			DEBUG_printf("No Valid ELF Signiture Located! (0x%x, %c, %c, %c)\n",
					Head->e_ident.ei_mag0, Head->e_ident.ei_mag1, Head->e_ident.ei_mag2,
					   Head->e_ident.ei_mag3);
	}
#endif

