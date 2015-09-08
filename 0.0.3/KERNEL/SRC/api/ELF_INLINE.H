/*
./SRC/ELF/ELF_INLINE.H
*/

#ifndef				ELF_INLINE_H_INCLUDED
	#define				ELF_INLINE_H_INCLUDED

	#include <api/api.h>

	// INLINE FUNCTIONS:

	static inline bool ELF_isSigned(ELF32_Head_p Head) {
		return ( ( (Head->e_ident.ei_mag0 == ELFMAG0) &&
			( Head->e_ident.ei_mag1 == ELFMAG1) &&
			  ( Head->e_ident.ei_mag2 == ELFMAG2) &&
			    ( Head->e_ident.ei_mag3 == ELFMAG3) ) ? TRUE : FALSE );
	}

	static inline bool ELF_is32BIT(ELF32_Head_p Head) {
		return ( ( Head->e_ident.ei_class == ELFCLASS32 ) ? TRUE : FALSE );
	}

	static inline bool ELF_isLittleEndian(ELF32_Head_p Head) {
		return ( ( Head->e_ident.ei_data == ELFDATA2LSB ) ? TRUE : FALSE );
	}

	static inline bool ELF_isCurrent(ELF32_Head_p Head) {
		return ( ( Head->e_ident.ei_version == EV_CURRENT ) ? TRUE : FALSE );
	}

	static inline bool ELF_is386(ELF32_Head_p Head) {
		return ( ( Head->e_machine == EM_386 ) ? TRUE : FALSE );
	}

	static inline bool ELF_isRelocatable(ELF32_Head_p Head) {
		return ( ( Head->e_type == ET_REL ) ? TRUE : FALSE );
	}

	static inline bool ELF_isExecutable(ELF32_Head_p Head) {
		return ( ( Head->e_type == ET_EXEC ) ? TRUE : FALSE );
	}

	static inline bool ELF_isSharedObject(ELF32_Head_p Head) {
		return ( ( Head->e_type == ET_DYN ) ? TRUE : FALSE );
	}

	static inline bool ELF_hasSHeadStringTable(ELF32_Head_p Head) {
		return ( ( Head->e_shstrndx == SHN_UNDEF ) ? FALSE : TRUE );
	}

	static inline Elf32_Word ELF_SHead_Count(ELF32_Head_p Head) {
		return ( (Head->e_shnum >= SHN_LORESERVE) ? (((ELF32_SHead_p) (Head + Head->e_shoff))->sh_size) :
				 Head->e_shnum );
	}

	static inline ELF32_SHead_p ELF_SHead_Base(ELF32_Head_p Head) {
		return (ELF32_SHead_p) ( (uint32_t)Head + Head->e_shoff );
	}

	static inline ELF32_SHead_p ELF_SHead(ELF32_Head_p Head, Elf32_Word Section) {
		return &ELF_SHead_Base(Head)[Section];
	}

	static inline void *ELF_Section(ELF32_Head_p Head, Elf32_Word Section) {
		return (void *) ((uint32_t) Head + (uint32_t) (ELF_SHead(Head, Section)->sh_offset));
	}

	static inline char *ELF_SHead_Get_Name(ELF32_Head_p Head, Elf32_Word Section) {
		return (char *) ( (Head->e_shstrndx) ? ((uint32_t) ELF_Section(Head, Head->e_shstrndx) +
				(ELF_SHead(Head, Section)->sh_name)) : (NULL) );
	}

	static inline char *ELF_Get_String(ELF32_Head_p Head, Elf32_Word Section, Elf32_Word offset) {
		Elf32_Word StrTbl = ELF_SHead(Head, Section)->sh_link;
		return (char *) ( (StrTbl) ? ((uint32_t) ELF_Section(Head, StrTbl) + offset) : (NULL) );
	}

	static inline int ELF_Get_SymVal(ELF32_Head_p Head, Elf32_Word Section, Elf32_Word offset) {
// DEBUG_printf("ELF_GET_SYSVAL...");
		ELF32_SHead_p symtab = ELF_SHead(Head, Section);
// DEBUG_printf("A");
		uint32_t symaddr = (uint32_t)Head + (uint32_t)symtab->sh_offset;
// DEBUG_printf("B");
		ELF32_SymTbl_p symbol = &(((ELF32_SymTbl_p)symaddr)[offset]);
// DEBUG_printf("C (0x%x)(0x%x)", (uint32_t) symbol, (uint32_t)symbol->st_shndx);
		if(symbol->st_shndx == SHN_UNDEF) {
// DEBUG_printf("D");
			ELF32_SHead_p strtab = ELF_SHead(Head, symtab->sh_link);
			const char *name = (const char *)Head + strtab->sh_offset + symbol->st_name;
// DEBUG_printf("SHN_UNDEF \"%s\"(0x%x)...", name, _API_Get_Symbol(name));
			void *target = (void*)_API_Get_Symbol(name);
			if(target)
				return (int)target;
#ifdef DEBUG
	DEBUG_printf("\nERROR Locating Function \"%s\"\n", name);
#endif
			return 0;
		} else if(symbol->st_shndx == SHN_ABS) {
// DEBUG_printf("E");
// DEBUG_printf("SHN_ABS (0x%x)...", (int) symbol->st_value);
			return (int) symbol->st_value;
		} else if(symbol->st_shndx == SHN_COMMON) {
#ifdef DEBUG
	DEBUG_printf("\nSHN_COMMON Entry Found... ERROR!\n");
#endif
			return 0;
		} else {
// DEBUG_printf("F");
			ELF32_SHead_p target = ELF_SHead(Head, symbol->st_shndx);
// DEBUG_printf("target = 0x%x...", (uint32_t) target);
// DEBUG_printf("ELSE (0x%x)...", ((int)Head + (int)symbol->st_value + (int)target->sh_offset));
			return ((int)Head + (int)symbol->st_value + (int)target->sh_offset);
		}
	}

	static inline ELF32_SHead_p ELF_SHead_By_String(ELF32_Head_p Head, const char * NAME) {
		for(uint16_t shd = 0; shd < ELF_SHead_Count(Head); shd++) {
			if(streql((const uint8_t*)ELF_SHead_Get_Name(Head, shd), (const uint8_t*)NAME))
				return ELF_SHead(Head, shd);
		}
		return NULL;
	}

	static inline uint32_t ELF_Get_Entry(void *File) {
		ELF32_Head_p Head = (ELF32_Head_p) File;
		if (Head->e_entry) {
			if(Head->e_entry > (uint32_t)File)
				return (uint32_t) Head->e_entry;
			else
				return (uint32_t) ((uint32_t)File + (uint32_t)ELF_SHead_By_String(Head, ".text")->sh_offset + Head->e_entry);
		} else {
			ELF32_SHead_p SHead;
			if((SHead = ELF_SHead_By_String(Head, ".text.startup")))
				return (uint32_t) ((uint32_t)File + (uint32_t)SHead->sh_offset);
			else if((SHead = ELF_SHead_By_String(Head, ".text")))
				return (uint32_t) ((uint32_t)File + (uint32_t)SHead->sh_offset);
			else
				return NULL;
		}

	}


/*
	static inline uint32_t ELF_SHead_Type(ELF32_SHead_p SHead) {
		return SHead->sh_type;
	}

	static inline uint32_t ELF_SHead_Name(ELF32_SHead_p SHead) {
		return SHead->sh_name;
	}

	static inline uint32_t ELF_SHead_Flags(ELF32_SHead_p SHead) {
		return SHead->sh_flags;
	}

	static inline uint32_t ELF_SHead_Address(ELF32_SHead_p SHead) {
		return SHead->sh_addr;
	}

	static inline uint32_t ELF_SHead_Offset(ELF32_SHead_p SHead) {
		return SHead->sh_offset;
	}

	static inline uint32_t ELF_SHead_Size(ELF32_SHead_p SHead) {
		return SHead->sh_size;
	}

	static inline uint32_t ELF_SHead_link(ELF32_SHead_p SHead) {
		return SHead->sh_link;
	}

	static inline uint32_t ELF_SHead_info(ELF32_SHead_p SHead) {
		return SHead->sh_info;
	}

	static inline uint32_t ELF_SHead_AddrAlign(ELF32_SHead_p SHead) {
		return SHead->sh_addralign;
	}

	static inline uint32_t ELF_SHead_EntSize(ELF32_SHead_p SHead) {
		return SHead->sh_entsize;
	}
*/

#endif