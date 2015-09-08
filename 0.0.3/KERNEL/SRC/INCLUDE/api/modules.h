/*
./SRC/INCLUDE/API.h
*/

#ifndef				MODULES_H_INCLUDED
	#define				MODULES_H_INCLUDED

	#include <KLIB.h>
	#include <BOSBOOT.h>

	bool Load_Mod(void* ELF_Loc);
	void _BTM_init(BootInfo_p BOOTINF);

#endif