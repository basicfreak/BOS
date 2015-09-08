/*
./SRC/API/modules.c
*/

#include <api/modules.h>
#include <api/elf.h>
#include <api/api.h>

bool Load_Mod(void* ELF_Loc)
{
	/*void (*_INIT_)(uint32_t API_GET) = (void(*)(uint32_t)) Load_ELF(ELF_Loc);
	if(_INIT_) {
		_INIT_((uint32_t)&_API_Get_Symbol);
		return TRUE;
	}*/
// DEBUG_printf("Load_Mod...");
	int (*_INIT_)(void) = (int(*)(void)) Load_ELF(ELF_Loc);
	if(_INIT_) {
// DEBUG_printf("RUN_MOD...");
		_INIT_();
// DEBUG_printf("DONE!\n");
		return TRUE;
	}

// DEBUG_printf("ERROR!\n");
	return FALSE;
}

void _BTM_init(BootInfo_p BOOTINF)
{
	ModL_p ModList = BOOTINF->ModList;
	for (uint8_t mod = 0; mod < BOOTINF->ModCount; mod++)
		Load_Mod((void *) ModList[mod].ModStart);
}
