/*
./SRC/INCLUDE/ELF.H
*/

#ifndef				ELF_H_INCLUDED
	#define				ELF_H_INCLUDED
	
	#include <KLIB.h>

	#ifdef DEBUG
		void ELF_Display_Data(void *File);
	#endif

	uint32_t Load_ELF(void *File);

#endif