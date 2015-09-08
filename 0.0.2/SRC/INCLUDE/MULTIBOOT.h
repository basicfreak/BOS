/*
./INCLUDE/MULTIBOOT.H
*/
#ifndef				MULTIBOOT_H_INCLUDED
	#define				MULTIBOOT_H_INCLUDED

	#include <KLIB.h>

	#define MULTIBOOT_MAGIC							0x2BADB002
	//MULTIBOOT FLAGS
	#define MULTIBOOT_INFO_MEMORY					0x00000001
	#define MULTIBOOT_INFO_BOOTDEV					0x00000002
	#define MULTIBOOT_INFO_CMDLINE					0x00000004
	#define MULTIBOOT_INFO_MODS						0x00000008
	#define MULTIBOOT_INFO_AOUT_SYMS				0x00000010
	#define MULTIBOOT_INFO_ELF_SHDR					0X00000020
	#define MULTIBOOT_INFO_MEM_MAP					0x00000040
	#define MULTIBOOT_INFO_DRIVE_INFO				0x00000080
	#define MULTIBOOT_INFO_CONFIG_TABLE				0x00000100
	#define MULTIBOOT_INFO_BOOT_LOADER_NAME			0x00000200
	#define MULTIBOOT_INFO_APM_TABLE				0x00000400
	#define MULTIBOOT_INFO_VBE_INFO					0x00000800
	#define MULTIBOOT_INFO_FRAMEBUFFER_INFO			0x00001000

	//MULTIBOOT FRAMEBUFFER TYPES
	#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED		0
	#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB			1
	#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT		2

	//MULTIBOOT MEMORY MAP TYPES
	#define MULTIBOOT_MEMORY_AVAILABLE				1
	#define MULTIBOOT_MEMORY_RESERVED				2
	#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE		3
	#define MULTIBOOT_MEMORY_NVS					4
	#define MULTIBOOT_MEMORY_BADRAM					5


	/* The symbol table for a.out.  */
	typedef struct multiboot_aout_symbol_table
	{
		uint32_t tabsize;
		uint32_t strsize;
		uint32_t addr;
		uint32_t reserved;
	} multiboot_aout_symbol_table_t;

	/* The section header table for ELF.  */
	typedef struct multiboot_elf_section_header_table
	{
		uint32_t num;
		uint32_t size;
		uint32_t addr;
		uint32_t shndx;
	} multiboot_elf_section_header_table_t;

	// Main Multiboot Info
	typedef struct multiboot_info
	{
		/* Multiboot info version number */
		uint32_t flags;
		/* Available memory from BIOS */
		uint32_t mem_lower;
		uint32_t mem_upper;
		/* "root" partition */
		uint32_t boot_device;
		/* Kernel command line */
		uint32_t cmdline;
		/* Boot-Module list */
		uint32_t mods_count;
		uint32_t mods_addr;
		union
		{
			multiboot_aout_symbol_table_t aout_sym;
			multiboot_elf_section_header_table_t elf_sec;
		} u;
		/* Memory Mapping buffer */
		uint32_t mmap_length;
		uint32_t mmap_addr;
		/* Drive Info buffer */
		uint32_t drives_length;
		uint32_t drives_addr;
		/* ROM configuration table */
		uint32_t config_table;
		/* Boot Loader Name */
		uint32_t boot_loader_name;
		/* APM table */
		uint32_t apm_table;
		/* Video */
		uint32_t vbe_control_info;
		uint32_t vbe_mode_info;
		uint16_t vbe_mode;
		uint16_t vbe_interface_seg;
		uint16_t vbe_interface_off;
		uint16_t vbe_interface_len;
		uint64_t framebuffer_addr;
		uint32_t framebuffer_pitch;
		uint32_t framebuffer_width;
		uint32_t framebuffer_height;
		uint8_t framebuffer_bpp;
		uint8_t framebuffer_type;
		union
		{
		struct
			{
				uint32_t framebuffer_palette_addr;
				uint16_t framebuffer_palette_num_colors;
			} multiboot_framebuffer_palette_t;
			struct
			{
				uint8_t framebuffer_red_field_position;
				uint8_t framebuffer_red_mask_size;
				uint8_t framebuffer_green_field_position;
				uint8_t framebuffer_green_mask_size;
				uint8_t framebuffer_blue_field_position;
				uint8_t framebuffer_blue_mask_size;
			} multiboot_framebuffer_color_t;
		} multiboot_framebuffer_t;
	} multiboot_info_t;

	typedef struct multiboot_mmap_entry
	{
		uint32_t size;
		uint64_t addr;
		uint64_t len;
		uint32_t type;
	} __attribute__((packed)) multiboot_memory_map_t;

	typedef struct multiboot_mod_list
	{
		/* the memory used goes from bytes 'mod_start' to 'mod_end-1' inclusive */
		uint32_t mod_start;
		uint32_t mod_end;
		/* Module command line */
		uint32_t cmdline;
		/* padding to take it to 16 bytes (must be zero) */
		uint32_t pad;
	} multiboot_module_t;

#endif