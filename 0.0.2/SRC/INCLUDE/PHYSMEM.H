/*
./SRC/INCLUDE/PHYSMEM.H
*/

#ifndef				PHYSMEM_H_INCLUDED
	#define				PHYSMEM_H_INCLUDED

	#include <BOSBOOT.h>

	typedef struct Physical_Memory_Struct
	{
		uint16_t	Location_HighMem_High;
		uint8_t		Location_HighMem_Low;
		uint8_t		Location_LowMem;
		uint16_t	Size_HighMem_High;
		uint8_t		Size_HighMem_Low;
		uint8_t		Size_LowMem;
	} __attribute__((packed)) PHYSMEM_ENT_t, *PHYSMEM_ENT_p;

	typedef struct Physical_Memory_Table
	{
		PHYSMEM_ENT_t ENTRY[64];
	} __attribute__((packed)) PHYSMEM_t, *PHYSMEM_p;

	bool		PHYSMEM_init			(BootInfo_p BOOTINF);
	void		*malloc_page			(uint32_t pages);
	void		*calloc_page			(uint32_t pages);
	void		*malloc_page_low		(uint32_t pages);
	void		*calloc_page_low		(uint32_t pages);
	void		free_page				(void *page, uint32_t pages);
	uint32_t	get_TotalMem			(void);

	#ifdef DEBUG
		void PHYSMEM_Display_Stack(void);
	#endif

#endif