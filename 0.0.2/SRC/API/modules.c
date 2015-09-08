/*
./SRC/API/modules.c
*/

#include <modules.h>
#include <ELF.h>
#include <API.h>

bool Load_Mod(void* ELF_Loc)
{
	/*void (*_INIT_)(uint32_t API_GET) = (void(*)(uint32_t)) Load_ELF(ELF_Loc);
	if(_INIT_) {
		_INIT_((uint32_t)&_API_Get_Symbol);
		return TRUE;
	}*/

	int (*_INIT_)(void) = (int(*)(void)) Load_ELF(ELF_Loc);
	if(_INIT_) {
		_INIT_();
		return TRUE;
	}
	return FALSE;
}