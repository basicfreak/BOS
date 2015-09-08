/*
./SRC/INCLUDE/API.h
*/

#ifndef				API_H_INCLUDED
	#define				API_H_INCLUDED

	#include <KLIB.h>

	typedef struct Block_Device_Struct
	{
		void (*open)(void *Options);
		void (*close)(void);
		void (*read)(uint32_t address, uint32_t size, void *buffer);
		void (*write)(uint32_t address, uint32_t size, void *buffer);
	} __attribute__((packed)) API_BLOCKDEV_t, *API_BLOCKDEV_p;

	typedef struct Module_Struct
	{
		uint32_t ID;
		API_BLOCKDEV_p BDEV;
	} __attribute__((packed)) API_MOD_t, *API_MOD_p;

	typedef struct SymTbl_Struct
	{
		char *NAME;
		void *FUNCTADDR;
	} __attribute__((packed)) API_SymTbl_t, *API_SymTbl_p;

	void _API_init(void);
	bool _API_Add_Symbol(const char* Name, uint32_t Funct);
	uint32_t _API_Get_Symbol(const char* Name);
	void INT(uint8_t num, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);


#endif