/*
./SRC/ELF/ELF_DATA.H
*/

#ifndef				ELF_DATA_H_INCLUDED
	#define				ELF_DATA_H_INCLUDED

	#include <typedefines.h>

// Define ELFxx_SymTbl_p->info macros:
	#define ELF_ST_BIND(i)			((i)>>4)
	#define ELF_ST_TYPE(i)			((i)&0xF)
	#define ELF_ST_INFO(b,t)		(((b)<<4)+((t)&0xF))

// Define ELFxx_SymTbl_p->other (Visibility) macros:
	#define ELF_ST_VISIBILITY(o)	((o)&0x3)

// 32-BIT ELF Relocation Info macros:
	#define ELF32_R_SYM(i)			((i)>>8)
	#define ELF32_R_TYPE(i)			((unsigned char)(i))
	#define ELF32_R_INFO(s,t)		(((s)<<8)+(unsigned char)(t))

// 64-BIT ELF Relocation Info macros:
	#define ELF64_R_SYM(i)			((i)>>32)
	#define ELF64_R_TYPE(i)			((i)&0xFFFFFFFFL)
	#define ELF64_R_INFO(s,t)		(((s)<<32)+((t)&0xFFFFFFFFL))

	#define DO_386_32(S, A)			((S) + (A))
	#define DO_386_PC32(S, A, P)	((S) + (A) - (P))

// 32-BIT ELF Data Types:
	typedef uint16_t		Elf32_Half;		// 2 Bytes
	typedef uint32_t		Elf32_Off;		// 4 Bytes
	typedef uint32_t		Elf32_Addr;		// 4 Bytes
	typedef uint32_t		Elf32_Word;		// 4 Bytes
	typedef int32_t			Elf32_Sword;	// 4 Bytes (Signed)
// 64-BIT ELF Data Types:
	typedef uint64_t		Elf64_Addr;		// 8 Bytes
	typedef uint64_t		Elf64_Off;		// 8 Bytes
	typedef uint16_t		Elf64_Half;		// 2 Bytes
	typedef uint32_t		Elf64_Word;		// 4 Bytes
	typedef int32_t			Elf64_Sword;	// 4 Bytes (Signed)
	typedef uint64_t		Elf64_Xword;	// 8 Bytes
	typedef int64_t			Elf64_Sxword;	// 8 Bytes (Signed)

// ELF COMMON IDENT:
	typedef struct 			ELF_IDENT {		// 16 Bytes
		uint8_t				ei_mag0;		// 1
		char				ei_mag1;		// 2
		char				ei_mag2;		// 3
		char				ei_mag3;		// 4
		uint8_t				ei_class;		// 5
		uint8_t				ei_data;		// 6
		uint8_t				ei_version;		// 7
		uint8_t				ei_osabi;		// 8
		uint8_t				ei_abiversion;	// 9
		uint8_t				ei_pad[7];		// 10 - 16
	}						ELF_IDENT_t, *ELF_IDENT_p;

// 32-BIT ELF Header:
	typedef struct			ELF32_HEAD {	// 52 Bytes
		ELF_IDENT_t			e_ident;		// 1 - 16
		Elf32_Half			e_type;			// 17, 18
		Elf32_Half			e_machine;		// 19, 20
		Elf32_Word			e_version;		// 21 - 24
		Elf32_Addr			e_entry;		// 25 - 28
		Elf32_Off			e_phoff;		// 29 - 32
		Elf32_Off			e_shoff;		// 33 - 36
		Elf32_Word			e_flags;		// 37 - 40
		Elf32_Half			e_ehsize;		// 41, 42
		Elf32_Half			e_phentsize;	// 43, 44
		Elf32_Half			e_phnum;		// 45, 46
		Elf32_Half			e_shentsize;	// 47, 48
		Elf32_Half			e_shnum;		// 49, 50
		Elf32_Half			e_shstrndx;		// 51, 52
	}						Elf32_Ehdr, ELF32_Head_t, *ELF32_Head_p;

// 64-BIT ELF Header:
	typedef struct			ELF64_HEAD {	// 64 Bytes
		ELF_IDENT_t			e_ident;		// 1 - 16
		Elf64_Half			e_type;			// 17, 18
		Elf64_Half			e_machine;		// 19, 20
		Elf64_Word			e_version;		// 21 - 24
		Elf64_Addr			e_entry;		// 25 - 32
		Elf64_Off			e_phoff;		// 33 - 40
		Elf64_Off			e_shoff;		// 41 - 48
		Elf64_Word			e_flags;		// 49 - 52
		Elf64_Half			e_ehsize;		// 53, 54
		Elf64_Half			e_phentsize;	// 55, 56
		Elf64_Half			e_phnum;		// 57, 58
		Elf64_Half			e_shentsize;	// 59, 60
		Elf64_Half			e_shnum;		// 61, 62
		Elf64_Half			e_shstrndx;		// 63, 64
	}						Elf64_Ehdr, ELF64_Head_t, *ELF64_Head_p;

// 32-BIT ELF Section Header:
	typedef struct			ELF32_SHead {	// 40 Bytes
		Elf32_Word			sh_name;		// 1 - 4
		Elf32_Word			sh_type;		// 5 - 8
		Elf32_Word			sh_flags;		// 9 - 12
		Elf32_Addr			sh_addr;		// 13 - 16
		Elf32_Off			sh_offset;		// 17 - 20
		Elf32_Word			sh_size;		// 21 - 24
		Elf32_Word			sh_link;		// 25 - 28
		Elf32_Word			sh_info;		// 29 - 32
		Elf32_Word			sh_addralign;	// 33 - 36
		Elf32_Word			sh_entsize;		// 37 - 40
	}						Elf32_Shdr, ELF32_SHead_t, *ELF32_SHead_p;

// 64-BIT ELF Section Header:
	typedef struct			ELF64_SHead {	// 64 Bytes
		Elf64_Word			sh_name;		// 1 - 4
		Elf64_Word			sh_type;		// 5 - 8
		Elf64_Xword			sh_flags;		// 9 - 16
		Elf64_Addr			sh_addr;		// 17 - 24
		Elf64_Off			sh_offset;		// 25 - 32
		Elf64_Xword			sh_size;		// 33 - 40
		Elf64_Word			sh_link;		// 41 - 44
		Elf64_Word			sh_info;		// 45 - 48
		Elf64_Xword			sh_addralign;	// 49 - 56
		Elf64_Xword			sh_entsize;		// 57 - 64
	}						Elf64_Shdr, ELF64_SHead_t, *ELF64_SHead_p;

// 32-BIT ELF Symbol Table:
	typedef struct			ELF32_SymTbl {	// 16 Bytes
		Elf32_Word			st_name;		// 1 - 4
		Elf32_Addr			st_value;		// 5 - 8
		Elf32_Word			st_size;		// 9 - 12
		unsigned char		st_info;		// 13
		unsigned char		st_other;		// 14
		Elf32_Half			st_shndx;		// 15, 16
	}						Elf32_Sym, ELF32_SymTbl_t, *ELF32_SymTbl_p;

// 64-BIT ELF Symbol Table:
	typedef struct			ELF64_SymTbl {	// 24 Bytes
		Elf64_Word			st_name;		// 1 - 4
		unsigned char		st_info;		// 5
		unsigned char		st_other;		// 6
		Elf64_Half			st_shndx;		// 7, 8
		Elf64_Addr			st_value;		// 9 - 16
		Elf64_Xword			st_size;		// 17 - 24
	}						Elf64_Sym, ELF64_SymTbl_t, *ELF64_SymTbl_p;

// 32-BIT ELF Reloctation Table WO/ Addends
	typedef struct			ELF32_RelTbl {	// 8 Bytes
		Elf32_Addr			r_offset;		// 1 - 4		Offset From Beginning Of Section
		Elf32_Word			r_info;			// 5 - 8		
	}						Elf32_Rel, ELF32_Rel_t, *ELF32_Rel_p;

// 32-BIT ELF Reloctation Table W/ Addends:
	typedef struct			ELF32_RelaTbl {	// 12 Bytes
		Elf32_Addr			r_offset;		// 1 - 4		Offset From Beginning Of Section
		Elf32_Word			r_info;			// 5 - 8
		Elf32_Sword			r_addend;		// 9 - 12		Constant Addend
	}						Elf32_Rela, ELF32_Rela_t, *ELF32_Rela_p;

// 64-BIT ELF Reloctation Table WO/ Addends:
	typedef struct			ELF64_RelTbl {	// 16 Bytes
		Elf64_Addr			r_offset;		// 1 - 8		Offset From Beginning Of Section
		Elf64_Xword			r_info;			// 9 - 16
	}						Elf64_Rel, ELF64_Rel_t, *ELF64_Rel_p;

// 64-BIT ELF Reloctation Table W/ Addends:
	typedef struct			ELF64_RelaTbl {	// 24 Bytes
		Elf64_Addr			r_offset;		// 1 - 8		Offset From Beginning Of Section
		Elf64_Xword			r_info;			// 9 - 16
		Elf64_Sxword		r_addend;		// 17 - 24		Constant Addend
	}						Elf64_Rela, ELF64_Rela_t, *ELF64_Rela_p;

// 32-BIT Program Header:
	typedef struct 			ELF32_PHead {	// 32 Bytes
		Elf32_Word			p_type;			// 1 - 4
		Elf32_Off			p_offset;		// 5 - 8
		Elf32_Addr			p_vaddr;		// 9 - 12
		Elf32_Addr			p_paddr;		// 13 - 16
		Elf32_Word			p_filesz;		// 17 - 20
		Elf32_Word			p_memsz;		// 21 - 24
		Elf32_Word			p_flags;		// 25 - 28
		Elf32_Word			p_align;		// 29 - 32
	}						Elf32_Phdr, ELF32_PHead_t, *ELF32_PHead_p; 

// 64-BIT Program Header:
	typedef struct 			ELF64_PHead {	// 56 Bytes
		Elf64_Word			p_type;			// 1 - 4
		Elf64_Word			p_flags;		// 5 - 8
		Elf64_Off			p_offset;		// 9 - 16
		Elf64_Addr			p_vaddr;		// 17 - 24
		Elf64_Addr			p_paddr;		// 25 - 32
		Elf64_Xword			p_filesz;		// 33 - 40
		Elf64_Xword			p_memsz;		// 47 - 48
		Elf64_Xword			p_align;		// 49 - 56
	}						Elf64_Phdr, ELF64_PHead_t, *ELF64_PHead_p;

// 32-BIT Dynamic Section:
	typedef struct 			ELF32_Dynamic {	// 12 Bytes
		Elf32_Sword			d_tag;			// 1 - 4
		union {
			Elf32_Word		d_val;			// 5 - 8
			Elf32_Addr		d_ptr;			// 9 - 12
		} d_un;
	}						Elf32_Dyn, ELF32_Dym_t, *ELF32_Dym_p;

// 64-BIT Dynamic Section:
	typedef struct 			ELF64_Dynamic {	// 24 Bytes
		Elf64_Sxword		d_tag;			// 1 - 8
		union {
			Elf64_Xword		d_val;			// 9 - 16
			Elf64_Addr		d_ptr;			// 17 - 24
		} d_un;
	}						Elf64_Dyn, ELF64_Dym_t, *ELF64_Dym_p;

// ELFxx_HEAD_p->e_type Values:
	enum e_type_vars {
		ET_NONE				= 0,			// No file type
		ET_REL				= 1,			// Relocatable file
		ET_EXEC				= 2,			// Executable file
		ET_DYN				= 3,			// Shared object file
		ET_CORE				= 4,			// Core file
		ET_LOOS				= 0xFE00,		// Operating system-specific
		ET_HIOS				= 0xFEFF,		// Operating system-specific
		ET_LOPROC			= 0xFF00,		// Processor-specific
		ET_HIPROC			= 0xFFFF		// Processor-specific
	};

// ELFxx_HEAD_p->e_machine Values:
	enum e_machine_vars {
		EM_NONE				= 0,			// No machine
		EM_M32				= 1,			// AT&T WE 32100
		EM_SPARC			= 2,			// SPARC
		EM_386				= 3,			// Intel 80386
		EM_68K				= 4,			// Motorola 68000
		EM_88K				= 5,			// Motorola 88000
		EM_RES0				= 6,			// RESERVED
		EM_860				= 7,			// Intel 80860
		EM_MIPS				= 8,			// MIPS I Architecture
		EM_S370				= 9,			// IBM System/370 Processor
		EM_MIPS_RS3_LE		= 0xA,			// MIPS RS3000 Little-endian
		EM_RES1				= 0xB,			// RESERVED
		EM_RES2				= 0xC,			// RESERVED
		EM_RES3				= 0xD,			// RESERVED
		EM_RES4				= 0xE,			// RESERVED
		EM_PARISC			= 0xF,			// Hewlett-Packard PA-RISC
		EM_RES5				= 0x10,			// RESERVED
		EM_VPP500			= 0x11,			// Fujitsu VPP500
		EM_SPARC32PLUS		= 0x12,			// Enhanced instruction set SPARC
		EM_960				= 0x13,			// Intel 80960
		EM_PPC				= 0x14,			// PowerPC
		EM_PPC64			= 0x15,			// 64-bit PowerPC
		EM_S390				= 0x16,			// IBM System/390 Processor
		EM_RES6				= 0x17,			// RESERVED
		EM_RES7				= 0x18,			// RESERVED
		EM_RES8				= 0x19,			// RESERVED
		EM_RES9				= 0x1A,			// RESERVED
		EM_RESA				= 0x1B,			// RESERVED
		EM_RESB				= 0x1C,			// RESERVED
		EM_RESC				= 0x1D,			// RESERVED
		EM_RESD				= 0x1E,			// RESERVED
		EM_RESE				= 0x1F,			// RESERVED
		EM_RESF				= 0x20,			// RESERVED
		EM_RES10			= 0x21,			// RESERVED
		EM_RES11			= 0x22,			// RESERVED
		EM_RES12			= 0x23,			// RESERVED
		EM_V800				= 0x24,			// NEC V800
		EM_FR20				= 0x25,			// Fujitsu FR20
		EM_RH32				= 0x26,			// TRW RH-32
		EM_RCE				= 0x27,			// Motorola RCE
		EM_ARM				= 0x28,			// Advanced RISC Machines ARM
		EM_ALPHA			= 0x29,			// Digital Alpha
		EM_SH				= 0x2A,			// Hitachi SH
		EM_SPARCV9			= 0x2B,			// SPARC Version 9
		EM_TRICORE			= 0x2C,			// Siemens TriCore embedded processor
		EM_ARC				= 0x2D,			// Argonaut RISC Core, Argonaut Technologies Inc.
		EM_H8_300			= 0x2E,			// Hitachi H8/300
		EM_H8_300H			= 0x2F,			// Hitachi H8/300H
		EM_H8S				= 0x30,			// Hitachi H8S
		EM_H8_500			= 0x31,			// Hitachi H8/500
		EM_IA_64			= 0x32,			// Intel IA-64 processor architecture
		EM_MIPS_X			= 0x33,			// Stanford MIPS-X
		EM_COLDFIRE			= 0x34,			// Motorola ColdFire
		EM_68HC12			= 0x35,			// Motorola M68HC12
		EM_MMA				= 0x36,			// Fujitsu MMA Multimedia Accelerator
		EM_PCP				= 0x37,			// Siemens PCP
		EM_NCPU				= 0x38,			// Sony nCPU embedded RISC processor
		EM_NDR1				= 0x39,			// Denso NDR1 microprocessor
		EM_STARCORE			= 0x3A,			// Motorola Star*Core processor
		EM_ME16				= 0x3B,			// Toyota ME16 processor
		EM_ST100			= 0x3C,			// STMicroelectronics ST100 processor
		EM_TINYJ			= 0x3D,			// Advanced Logic Corp. TinyJ embedded processor family
		EM_X86_64			= 0x3E,			// AMD x86-64 architecture
		EM_PDSP				= 0x3F,			// Sony DSP Processor
		EM_PDP10			= 0x40,			// Digital Equipment Corp. PDP-10
		EM_PDP11			= 0x41,			// Digital Equipment Corp. PDP-11
		EM_FX66				= 0x42,			// Siemens FX66 microcontroller
		EM_ST9PLUS			= 0x43,			// STMicroelectronics ST9+ 8/16 bit microcontroller
		EM_ST7				= 0x44,			// STMicroelectronics ST7 8-bit microcontroller
		EM_68HC16			= 0x45,			// Motorola MC68HC16 Microcontroller
		EM_68HC11			= 0x46,			// Motorola MC68HC11 Microcontroller
		EM_68HC08			= 0x47,			// Motorola MC68HC08 Microcontroller
		EM_68HC05			= 0x48,			// Motorola MC68HC05 Microcontroller
		EM_SVX				= 0x49,			// Silicon Graphics SVx
		EM_ST19				= 0x4A,			// STMicroelectronics ST19 8-bit microcontroller
		EM_VAX				= 0x4B,			// Digital VAX
		EM_CRIS				= 0x4C,			// Axis Communications 32-bit embedded processor
		EM_JAVELIN			= 0x4D,			// Infineon Technologies 32-bit embedded processor
		EM_FIREPATH			= 0x4E,			// Element 14 64-bit DSP Processor
		EM_ZSP				= 0x4F,			// LSI Logic 16-bit DSP Processor
		EM_MMIX				= 0x50,			// Donald Knuth's educational 64-bit processor
		EM_HUANY			= 0x51,			// Harvard University machine-independent object files
		EM_PRISM			= 0x52,			// SiTera Prism
		EM_AVR				= 0x53,			// Atmel AVR 8-bit microcontroller
		EM_FR30				= 0x54,			// Fujitsu FR30
		EM_D10V				= 0x55,			// Mitsubishi D10V
		EM_D30V				= 0x56,			// Mitsubishi D30V
		EM_V850				= 0x57,			// NEC v850
		EM_M32R				= 0x58,			// Mitsubishi M32R
		EM_MN10300			= 0x59,			// Matsushita MN10300
		EM_MN10200			= 0x5A,			// Matsushita MN10200
		EM_PJ				= 0x5B,			// picoJava
		EM_OPENRISC			= 0x5C,			// OpenRISC 32-bit embedded processor
		EM_ARC_A5			= 0x5D,			// ARC Cores Tangent-A5
		EM_XTENSA			= 0x5E,			// Tensilica Xtensa Architecture
		EM_VIDEOCORE		= 0x5F,			// Alphamosaic VideoCore processor
		EM_TMM_GPP			= 0x60,			// Thompson Multimedia General Purpose Processor
		EM_NS32K			= 0x61,			// National Semiconductor 32000 series
		EM_TPC				= 0x62,			// Tenor Network TPC processor
		EM_SNP1K			= 0x63,			// Trebia SNP 1000 processor
		EM_ST200			= 0x64,			// STMicroelectronics (www.st.com) ST200 microcontroller
		EM_IP2K				= 0x65,			// Ubicom IP2xxx microcontroller family
		EM_MAX				= 0x66,			// MAX Processor
		EM_CR				= 0x67,			// National Semiconductor CompactRISC microprocessor
		EM_F2MC16			= 0x68,			// Fujitsu F2MC16
		EM_MSP430			= 0x69,			// Texas Instruments embedded microcontroller msp430
		EM_BLACKFIN			= 0x6A,			// Analog Devices Blackfin (DSP) processor
		EM_SE_C33			= 0x6B,			// S1C33 Family of Seiko Epson processors
		EM_SEP				= 0x6C,			// Sharp embedded microprocessor
		EM_ARCA				= 0x6D,			// Arca RISC Microprocessor
		EM_UNICORE			= 0x6E			// Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University
	};

// ELFxx_HEAD_p->e_version Values:
	enum e_version_vars {
		EV_NONE				= 0,			// Invalid version
		EV_CURRENT			= 1				// Current version
	};

// ELFxx_HEAD_p->e_ident.ei_mag* Values:
	enum ei_mag_vars {
		ELFMAG0				= 0x7F,			// 1st Signiture Byte
		ELFMAG1				= 'E',			// 2nd Signiture Byte
		ELFMAG2				= 'L',			// 3rd Signiture Byte
		ELFMAG3				= 'F'			// 4th Signiture Byte
	};

// ELFxx_HEAD_p->e_ident.ei_class Values:
	enum ei_class_vars {
		ELFCLASSNONE		= 0,			// Invalid class
		ELFCLASS32			= 1,			// 32-bit objects
		ELFCLASS64			= 2				// 64-bit objects
	};

// ELFxx_HEAD_p->e_ident.ei_data Values:
	enum ei_data_vars {
		ELFDATANONE			= 0,			// Invalid data encoding
		ELFDATA2LSB			= 1,			// Little-Endian
		ELFDATA2MSB			= 2				// Big-Endian
	};

// ELFxx_HEAD_p->e_ident.ei_osabi Values:
	enum ei_osabi_vars {
		ELFOSABI_NONE		= 0,			// No extensions or unspecified
		ELFOSABI_HPUX		= 1,			// Hewlett-Packard HP-UX
		ELFOSABI_NETBSD		= 2,			// NetBSD
		ELFOSABI_SOLARIS	= 6,			// Sun Solaris
		ELFOSABI_AIX		= 7,			// AIX
		ELFOSABI_IRIX		= 8,			// IRIX
		ELFOSABI_FREEBSD	= 9,			// FreeBSD
		ELFOSABI_TRU64		= 0xA,			// Compaq TRU64 UNIX
		ELFOSABI_MODESTO	= 0xB,			// Novell Modesto
		ELFOSABI_OPENBSD	= 0xC,			// Open BSD
		ELFOSABI_OPENVMS	= 0xD,			// Open VMS
		ELFOSABI_NSK		= 0xE,			// Hewlett-Packard Non-Stop Kernel
		ELFOSABI_AROS		= 0xF 			// Amiga Research OS
	};

// ELFxx_HEAD_p->e_shnum Values:
	enum e_shnum_vars {
		SHN_UNDEF			= 0,			// Undefined, Missing, Irrelevant, Meaningless Section
		SHN_LORESERVE		= 0xFF00,		// Lower bound - Reserved Indexes
		SHN_LOPROC			= 0xFF00,		// Lower bound - Processor-Specific Semantics
		SHN_HIPROC			= 0xFF1F,		// Upper Bound - Processor-Specific Semantics
		SHN_LOOS			= 0xFF20,		// Lower bound - Operating System-Specific Semantics
		SHN_HIOS			= 0xFF3F,		// Upper Bound - Operating System-Specific Semantics
		SHN_ABS				= 0xFFF1,		// Absolute values - Not affected by relocation
		SHN_COMMON			= 0xFFF2,		// Symbols defined in Section are Common Symbols
		SHN_XINDEX			= 0xFFFF,		// Escape Value / Section Header Index is too Large
		SHN_HIRESERVE		= 0xFFFF		// Upper Bound - Reserved Indexes
	};

// ELFxx_SHEAD_p->sh_type Values:
	enum sh_type_vars {
		SHT_NULL			= 0,			// Inactive Section Header 
		SHT_PROGBITS		= 1,			// Program Section Header
		SHT_SYMTAB			= 2,			// Symbol Table Section Header
		SHT_STRTAB			= 3,			// String Table Section Header
		SHT_RELA			= 4,			// Relocation Table W/ Addends Section Header
		SHT_HASH			= 5,			// Symbol Hash Table Section Header
		SHT_DYNAMIC			= 6,			// Dynamic Linking Table Section Header
		SHT_NOTE			= 7,			// Notes Section Header
		SHT_NOBITS			= 8,			// Section Occupies NO File Space
		SHT_REL				= 9,			// Relocation Table WO/ Addends Section Header
		SHT_SHLIB			= 0xA,			// Reserved? Section Header
		SHT_DYNSYM			= 0xB,			// Dynamic Linking Symbol Table Section Header
		SHT_INIT_ARRAY		= 0xE,			// Initialization Function Pointer Array Section Header
		SHT_FINI_ARRAY		= 0xF,			// Termination Function Pointer Array Section Header
		SHT_PREINIT_ARRAY	= 0x10,			// Function Pointer Array Section Header
		SHT_GROUP			= 0x11,			// Group Section Header
		SHT_SYMTAB_SHNDX	= 0x12,			// Only if corresponding symbol table entry's shndx field contains the escape value SHN_XINDEX
		SHT_LOOS			= 0x60000000,	// Lower bound - Operating System-Specific Semantics
		SHT_HIOS			= 0x6FFFFFFF,	// Upper bound - Operating System-Specific Semantics
		SHT_LOPROC			= 0x70000000,	// Lower bound - Processor-Specific Semantics
		SHT_HIPROC			= 0x7FFFFFFF,	// Upper bound - Processor-Specific Semantics
		SHT_LOUSER			= -2147483648, //0x80000000,	// Lower bound - Application Programs
		SHT_HIUSER			= -1 //0xFFFFFFFF	// Upper bound - Application Programs
	};

// ELFxx_SHEAD_p->sh_flags Values:
	enum sh_flags_vars {
		SHF_NONE			= 0, 			// No Flags
		SHF_WRITE			= 1, 			// Writable During Process Execution
		SHF_ALLOC			= 2, 			// Occupies Memory During Process Execution
		SHF_EXECINSTR		= 4, 			// Executable Machine Instructions
		SHF_MERGE			= 0x10,			// 
		SHF_STRINGS			= 0x20,			// Null-Terminated Character Strings
		SHF_INFO_LINK		= 0x40,			// Section Header Table Index
		SHF_LINK_ORDER		= 0x80,			// Special Ordering Requirements for Link Editors
		SHF_OS_NONCONFORMING= 0x100, 		// OS-Specific Processing
		SHF_GROUP			= 0x200, 		// Member of a Section Group
		SHF_TLS				= 0x400, 		// Thread-Local Storage
		SHF_MASKOS			= 0x0ff00000, 	// Operating System-Specific Semantics
		SHF_MASKPROC		= -268435456 //0xf0000000 	// Processor-Specific Semantics
	};

// ELFxx_SHEAD_p->sh_group Values:
	enum sh_group_vars {
		GRP_COMDAT			= 1, 			// COMDAT Section
		GRP_MASKOS			= 0x0ff00000, 	// Operating System-Specific Semantics
		GRP_MASKPROC		= -268435456 //0xf0000000	// Processor-Specific Semantics
	};

// ELFxx_SHEAD_p->sh_name Values:
	enum st_name_vars {
		STN_UNDEF			= 0 			// Name Undefined
	};

// ELF_ST_BIND(i) Values:
	enum st_bind_vars {
		STB_LOCAL			= 0, 			// Local Symbols
		STB_GLOBAL			= 1, 			// Global Symbols
		STB_WEAK			= 2, 			// Weak Symbols
		STB_LOOS			= 0xA, 			// Lower Bond - Operating System-Specific Semantics
		STB_HIOS			= 0xC, 			// Upper Bond - Operating System-Specific Semantics
		STB_LOPROC			= 0xD, 			// Lower Bond - Processor-Specific Semantics
		STB_HIPROC			= 0xF 			// Upper Bond - Processor-Specific Semantics
	};

// ELF_ST_TYPE(i) Values:
	enum st_type_vars {
		STT_NOTYPE			= 0, 			// Symbol's Type Not Specified
		STT_OBJECT			= 1, 			// Data Object
		STT_FUNC			= 2, 			// Function or Other Executable Code
		STT_SECTION			= 3, 			// Section
		STT_FILE			= 4, 			// Symbol's Name - Source File of Object File
		STT_COMMON			= 5, 			// Uninitialized Common Block
		STT_TLS				= 6, 			// Thread-Local Storage Entity
		STT_LOOS			= 0xA, 			// Lower Bond - Operating System-Specific Semantics
		STT_HIOS			= 0xC, 			// Upper Bond - Operating System-Specific Semantics
		STT_LOPROC			= 0xD, 			// Lower Bond - Processor-Specific Semantics
		STT_HIPROC			= 0xF 			// Upper Bond - Processor-Specific Semantics
	};

// ELF_ST_VISIBILITY(o) Values:
	enum st_visibility_vars {
		STV_DEFAULT			= 0, 			// Default (Visible)
		STV_INTERNAL		= 1, 			// Internel (Hidden)
		STV_HIDDEN			= 2, 			// Hidden (duh... Hidden)
		STV_PROTECTED		= 3 			// Protected  ?
	};

// ELFxx_PHead_p->p_type Values:
	enum p_type_vars {
		PT_NULL				= 0, 			// NULL / UNUSED / UNDEFINED / IGNORED
		PT_LOAD				= 1, 			// loadable segment
		PT_DYNAMIC			= 2, 			// dynamic linking information
		PT_INTERP			= 3, 			// null-terminated path name to invoke as an interpreter
		PT_NOTE				= 4, 			// auxiliary information
		PT_SHLIB			= 5, 			// reserved
		PT_PHDR				= 6, 			// location and size of the program header table
		PT_TLS				= 7, 			// Thread-Local Storage template
		PT_LOOS				= 0x60000000,	// Lower Bond - Operating System-Specific Semantics
		PT_HIOS				= 0x6FFFFFFF,	// Upper Bond - Operating System-Specific Semantics
		PT_LOPROC			= 0x70000000,	// Lower Bond - Processor-Specific Semantics
		PT_HIPROC			= 0x7FFFFFFF	// Upper Bond - Processor-Specific Semantics
	};

// ELFxx_PHead_p->p_flags Values:
	enum p_flags_vars {
		PF_NONE				= 0,			// All Access Denied
		PF_X				= 1, 			// Execute Access
		PF_W				= 2, 			// Write Access
		PF_R				= 4, 			// Read Access
		PF_MASKOS			= 0x0FF00000,	// Unspecified - Operating System-Specific
		PF_MASKPROC			= -268435456 //0xF0000000	// Unspecified - Processor-Specific
	};

// ELF64_Dym_p->d_tag Values:
	enum d_tag_vars {						// d_un				Executable		Shared Object
		DT_NULL				= 0, 			// IGNORED			mandatory		mandatory
		DT_NEEDED			= 1, 			// d_val			optional		optional
		DT_PLTRELSZ			= 2, 			// d_val			optional		optional
		DT_PLTGOT			= 3, 			// d_ptr			optional		optional
		DT_HASH				= 4, 			// d_ptr			mandatory		mandatory
		DT_STRTAB			= 5, 			// d_ptr			mandatory		mandatory
		DT_SYMTAB			= 6, 			// d_ptr			mandatory		mandatory
		DT_RELA				= 7, 			// d_ptr			mandatory		optional
		DT_RELASZ			= 8, 			// d_ptr			mandatory		optional
		DT_RELAENT			= 9, 			// d_val			mandatory		optional
		DT_STRSZ			= 0xA, 			// d_val			mandatory		mandatory
		DT_SYMENT			= 0xB, 			// d_val			mandatory		mandatory
		DT_INIT				= 0xC, 			// d_ptr			optional		optional
		DT_FINI				= 0xD, 			// d_ptr			optional		optional
		DT_SONAME			= 0xE, 			// d_val			ignored			optional
		DT_RPATH			= 0xF, 			// d_val			optional		ignored
		DT_SYMBOLIC			= 0x10, 		// ignored			ignored			optional
		DT_REL				= 0x11, 		// d_ptr			mandatory		optional*/
		DT_RELSZ			= 0x12, 		// d_val			mandatory		optional
		DT_RELENT			= 0x13, 		// d_val			mandatory		optional
		DT_PLTREL			= 0x14, 		// d_val			optional		optional
		DT_DEBUG			= 0x15, 		// d_ptr			optional		ignored
		DT_TEXTREL			= 0x16, 		// ignored			optional		optional
		DT_JMPREL			= 0x17, 		// d_ptr			optional		optional
		DT_BIND_NOW			= 0x18, 		// ignored			optional		optional
		DT_INIT_ARRAY		= 0x19, 		// d_ptr			optional		optional
		DT_FINI_ARRAY		= 0x1A, 		// d_ptr			optional		optional
		DT_INIT_ARRAYSZ		= 0x1B, 		// d_val			optional		optional
		DT_FINI_ARRAYSZ		= 0x1C, 		// d_val			optional		optional
		DT_RUNPATH			= 0x1D, 		// d_val			optional		optional
		DT_FLAGS			= 0x1E, 		// d_val			optional		optional
		DT_ENCODING			= 0x20, 		// unspecified		unspecified		unspecified
		DT_PREINIT_ARRAY	= 0x20, 		// d_ptr			optional		ignored
		DT_PREINIT_ARRAYSZ	= 0x21,			// d_val			optional		ignored
		DT_LOOS				= 0x6000000D,	// unspecified		unspecified		unspecified
		DT_HIOS				= 0x6FFFF000,	// unspecified		unspecified		unspecified
		DT_LOPROC			= 0x70000000,	// unspecified		unspecified		unspecified
		DT_HIPROC			= 0x7FFFFFFF	// unspecified		unspecified		unspecified
	};

// DT_FLAGS Values:
	enum dt_flags_vars {
		DF_ORIGIN			= 1, 			// $ORIGIN substitution string
		DF_SYMBOLIC			= 2, 			// symbol resolution algorithm
		DF_TEXTREL			= 4, 			// relocation entries might request modifications to a non-writable segment
		DF_BIND_NOW			= 8, 			// process all relocations for the object containing this entry before transferring control to the program
		DF_STATIC_TLS		= 0xA 			// contains code using a static thread-local storage scheme
	};

// Relocation Types
	enum RtT_Types {
		R_386_NONE		= 0, // No relocation
		R_386_32		= 1, // Symbol + Offset
		R_386_PC32		= 2  // Symbol + Offset - Section Offset
	};
	// #include "ELF_INLINE.H"

#endif

/***********RANDOM USEFUL INFORMATION ON ELF (DATA) FORMAT************

sh_type*************sh_link*************************************************************************sh_info****************************************************************************************************************************************************
SHT_DYNAMIC			The section header index of the string table used by entries in the section.	0
SHT_HASH			The section header index of the symbol table to which the hash table applies.	0
SHT_RELA			The section header index of the associated symbol table.						The section header index of the section to which the relocation applies.
SHT_RELA			The section header index of the associated symbol table.						The section header index of the section to which the relocation applies.
SHT_SYMTAB			The section header index of the associated string table.						One greater than the symbol table index of the last local symbol (binding STB_LOCAL).
SHT_DYNSYM			The section header index of the associated string table.						One greater than the symbol table index of the last local symbol (binding STB_LOCAL).
SHT_GROUP			The section header index of the associated symbol table.						The symbol table index of an entry in the associated symbol table. The name of the specified symbol table entry provides a signature for the section group.
SHT_SYMTAB_SHNDX	The section header index of the associated symbol table section.				0
***************************************************************************************************************************************************************************************************************************************************************

Name************Type****************Attributes*****************
.bss			SHT_NOBITS			SHF_ALLOC+SHF_WRITE
.comment		SHT_PROGBITS		none
.data			SHT_PROGBITS		SHF_ALLOC+SHF_WRITE
.data1			SHT_PROGBITS		SHF_ALLOC+SHF_WRITE
.debug			SHT_PROGBITS		none
.dynamic		SHT_DYNAMIC			see below
.dynstr			SHT_STRTAB			SHF_ALLOC
.dynsym			SHT_DYNSYM			SHF_ALLOC
.fini			SHT_PROGBITS		SHF_ALLOC+SHF_EXECINSTR
.fini_array		SHT_FINI_ARRAY		SHF_ALLOC+SHF_WRITE
.got			SHT_PROGBITS		see below
.hash			SHT_HASH			SHF_ALLOC
.init			SHT_PROGBITS		SHF_ALLOC+SHF_EXECINSTR
.init_array		SHT_INIT_ARRAY		SHF_ALLOC+SHF_WRITE
.interp			SHT_PROGBITS		see below
.line			SHT_PROGBITS		none
.note			SHT_NOTE			none
.plt			SHT_PROGBITS		see below
.preinit_array	SHT_PREINIT_ARRAY	SHF_ALLOC+SHF_WRITE
.relname		SHT_REL				see below
.relaname		SHT_RELA			see below
.rodata			SHT_PROGBITS		SHF_ALLOC
.rodata1		SHT_PROGBITS		SHF_ALLOC
.shstrtab		SHT_STRTAB			none
.strtab			SHT_STRTAB			see below
.symtab			SHT_SYMTAB			see below
.symtab_shndx	SHT_SYMTAB_SHNDX	see below
.tbss			SHT_NOBITS			SHF_ALLOC+SHF_WRITE+SHF_TLS
.tdata			SHT_PROGBITS		SHF_ALLOC+SHF_WRITE+SHF_TLS
.tdata1			SHT_PROGBITS		SHF_ALLOC+SHF_WRITE+SHF_TLS
.text			SHT_PROGBITS		SHF_ALLOC+SHF_EXECINSTR
***************************************************************
.dynamic
	This section holds dynamic linking information. The section's attributes will include the
	SHF_ALLOC bit. Whether the SHF_WRITE bit is set is processor specific. See Chapter 5 for more
	information.
.got
	This section holds the global offset table. See ``Coding Examples'' in Chapter 3, ``Special
	Sections'' in Chapter 4, and ``Global Offset Table'' in Chapter 5 of the processor supplement
	for more information.
.interp
	This section holds the path name of a program interpreter. If the file has a loadable segment
	that includes relocation, the sections' attributes will include the SHF_ALLOC bit; otherwise,
	that bit will be off. See Chapter 5 for more information.
.plt
	This section holds the procedure linkage table. See ``Special Sections'' in Chapter 4 and
	``Procedure Linkage Table'' in Chapter 5 of the processor supplement for more information.
.relname and .relaname
	These sections hold relocation information, as described in ``Relocation''. If the file has a
	loadable segment that includes relocation, the sections' attributes will include the SHF_ALLOC
	bit; otherwise, that bit will be off. Conventionally, name is supplied by the section to which
	the relocations apply. Thus a relocation section for .text normally would have the name
	.rel.text or .rela.text.
.strtab
	This section holds strings, most commonly the strings that represent the names associated with
	symbol table entries. If the file has a loadable segment that includes the symbol string table,
	the section's attributes will include the SHF_ALLOC bit; otherwise, that bit will be off.
.symtab
	This section holds a symbol table, as ``Symbol Table''. in this chapter describes. If the file
	has a loadable segment that includes the symbol table, the section's attributes will include
	the SHF_ALLOC bit; otherwise, that bit will be off.
.symtab_shndx
	This section holds the special symbol table section index array, as described above. The
	section's attributes will include the SHF_ALLOC bit if the associated symbol table section
	does; otherwise that bit will be off.



***********************END OF EXTRA INFORMATION**********************/