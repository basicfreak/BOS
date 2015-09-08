/*
./SRC/API/API.C
*/

#include <API.h>
#include <IDT.h>
#include <PHYSMEM.H>
#include <VIRTUALMEM.H>

#define API_MAX_ENT 4095
#define API_ENT_SIZE 8

uint32_t API_SysTbl_Ent = 0;
uint32_t API_SymTbl_Base;

void _API_init()
{
	API_SymTbl_Base = (uint32_t) calloc_page(2);

	_API_Add_Symbol("outb", (uint32_t)&outb);
	_API_Add_Symbol("inb", (uint32_t)&inb);
	_API_Add_Symbol("outw", (uint32_t)&outw);
	_API_Add_Symbol("inw", (uint32_t)&inw);
	_API_Add_Symbol("outl", (uint32_t)&outl);
	_API_Add_Symbol("inl", (uint32_t)&inl);
	_API_Add_Symbol("newHandler", (uint32_t)&newHandler);
	_API_Add_Symbol("strlen", (uint32_t)&strlen);
	_API_Add_Symbol("streql", (uint32_t)&streql);
	_API_Add_Symbol("strcmp", (uint32_t)&strcmp);
	_API_Add_Symbol("strcpy", (uint32_t)&strcpy);
	_API_Add_Symbol("stringf", (uint32_t)&stringf);
	_API_Add_Symbol("malloc_page", (uint32_t)&malloc_page);
	_API_Add_Symbol("calloc_page", (uint32_t)&calloc_page);
	_API_Add_Symbol("malloc_page_low", (uint32_t)&malloc_page_low);
	_API_Add_Symbol("calloc_page_low", (uint32_t)&calloc_page_low);
	_API_Add_Symbol("free_page", (uint32_t)&free_page);
	_API_Add_Symbol("AddPage", (uint32_t)&AddPage);
	_API_Add_Symbol("DelPage", (uint32_t)&DelPage);
	_API_Add_Symbol("_API_Add_Symbol", (uint32_t)&_API_Add_Symbol);
	_API_Add_Symbol("_API_Get_Symbol", (uint32_t)&_API_Get_Symbol);
	_API_Add_Symbol("DEBUG_printf", (uint32_t)&DEBUG_printf);
	_API_Add_Symbol("INT", (uint32_t)&INT);
	_API_Add_Symbol("memcpy", (uint32_t)&memcpy);
	_API_Add_Symbol("memset", (uint32_t)&memset);
	_API_Add_Symbol("memsetw", (uint32_t)&memsetw);

}

bool _API_Add_Symbol(const char* Name, uint32_t Funct)
{
#ifdef DEBUG_FULL
	DEBUG_printf("%i\t_API_Add_Symbol(\"%s\", 0x%x);\n", API_SysTbl_Ent, Name, Funct);
#endif
	if(API_SysTbl_Ent >= API_MAX_ENT)
		return FALSE;
	API_SymTbl_p SymEnt = (API_SymTbl_p) (API_SymTbl_Base + (API_SysTbl_Ent * API_ENT_SIZE));
	SymEnt->NAME = (char*) Name;
	SymEnt->FUNCTADDR = (void *) Funct;
	API_SysTbl_Ent++;
	return TRUE;
}

uint32_t _API_Get_Symbol(const char* Name)
{
	for(uint32_t x = 0; x < API_SysTbl_Ent; x++) {
		API_SymTbl_p SymEnt = (API_SymTbl_p) (API_SymTbl_Base + (x * API_ENT_SIZE));
		if(streql((const uint8_t*)SymEnt->NAME, (const uint8_t*)Name))
			return (uint32_t) SymEnt->FUNCTADDR;
	}
	return (uint32_t) 0;
}

void INT(uint8_t num, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
	if (num < 0x30)
		return;
	switch (num) {
		case 0x30:
			__asm__ __volatile__ ( "int $0x30" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x31:
			__asm__ __volatile__ ( "int $0x31" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x32:
			__asm__ __volatile__ ( "int $0x32" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x33:
			__asm__ __volatile__ ( "int $0x33" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x34:
			__asm__ __volatile__ ( "int $0x34" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x35:
			__asm__ __volatile__ ( "int $0x35" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x36:
			__asm__ __volatile__ ( "int $0x36" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x37:
			__asm__ __volatile__ ( "int $0x37" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x38:
			__asm__ __volatile__ ( "int $0x38" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x39:
			__asm__ __volatile__ ( "int $0x39" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x3A:
			__asm__ __volatile__ ( "int $0x3A" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x3B:
			__asm__ __volatile__ ( "int $0x3B" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x3C:
			__asm__ __volatile__ ( "int $0x3C" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x3D:
			__asm__ __volatile__ ( "int $0x3D" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x3E:
			__asm__ __volatile__ ( "int $0x3E" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x3F:
			__asm__ __volatile__ ( "int $0x3F" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x40:
			__asm__ __volatile__ ( "int $0x40" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x41:
			__asm__ __volatile__ ( "int $0x41" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x42:
			__asm__ __volatile__ ( "int $0x42" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x43:
			__asm__ __volatile__ ( "int $0x43" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x44:
			__asm__ __volatile__ ( "int $0x44" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x45:
			__asm__ __volatile__ ( "int $0x45" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x46:
			__asm__ __volatile__ ( "int $0x46" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x47:
			__asm__ __volatile__ ( "int $0x47" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x48:
			__asm__ __volatile__ ( "int $0x48" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x49:
			__asm__ __volatile__ ( "int $0x49" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x4A:
			__asm__ __volatile__ ( "int $0x4A" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x4B:
			__asm__ __volatile__ ( "int $0x4B" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x4C:
			__asm__ __volatile__ ( "int $0x4C" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x4D:
			__asm__ __volatile__ ( "int $0x4D" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x4E:
			__asm__ __volatile__ ( "int $0x4E" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x4F:
			__asm__ __volatile__ ( "int $0x4F" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x50:
			__asm__ __volatile__ ( "int $0x50" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x51:
			__asm__ __volatile__ ( "int $0x51" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x52:
			__asm__ __volatile__ ( "int $0x52" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x53:
			__asm__ __volatile__ ( "int $0x53" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x54:
			__asm__ __volatile__ ( "int $0x54" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x55:
			__asm__ __volatile__ ( "int $0x55" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x56:
			__asm__ __volatile__ ( "int $0x56" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x57:
			__asm__ __volatile__ ( "int $0x57" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x58:
			__asm__ __volatile__ ( "int $0x58" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x59:
			__asm__ __volatile__ ( "int $0x59" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x5A:
			__asm__ __volatile__ ( "int $0x5A" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x5B:
			__asm__ __volatile__ ( "int $0x5B" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x5C:
			__asm__ __volatile__ ( "int $0x5C" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x5D:
			__asm__ __volatile__ ( "int $0x5D" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x5E:
			__asm__ __volatile__ ( "int $0x5E" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x5F:
			__asm__ __volatile__ ( "int $0x5F" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x60:
			__asm__ __volatile__ ( "int $0x60" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x61:
			__asm__ __volatile__ ( "int $0x61" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x62:
			__asm__ __volatile__ ( "int $0x62" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x63:
			__asm__ __volatile__ ( "int $0x63" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x64:
			__asm__ __volatile__ ( "int $0x64" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x65:
			__asm__ __volatile__ ( "int $0x65" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x66:
			__asm__ __volatile__ ( "int $0x66" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x67:
			__asm__ __volatile__ ( "int $0x67" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x68:
			__asm__ __volatile__ ( "int $0x68" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x69:
			__asm__ __volatile__ ( "int $0x69" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x6A:
			__asm__ __volatile__ ( "int $0x6A" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x6B:
			__asm__ __volatile__ ( "int $0x6B" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x6C:
			__asm__ __volatile__ ( "int $0x6C" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x6D:
			__asm__ __volatile__ ( "int $0x6D" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x6E:
			__asm__ __volatile__ ( "int $0x6E" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x6F:
			__asm__ __volatile__ ( "int $0x6F" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x70:
			__asm__ __volatile__ ( "int $0x70" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x71:
			__asm__ __volatile__ ( "int $0x71" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x72:
			__asm__ __volatile__ ( "int $0x72" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x73:
			__asm__ __volatile__ ( "int $0x73" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x74:
			__asm__ __volatile__ ( "int $0x74" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x75:
			__asm__ __volatile__ ( "int $0x75" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x76:
			__asm__ __volatile__ ( "int $0x76" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x77:
			__asm__ __volatile__ ( "int $0x77" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x78:
			__asm__ __volatile__ ( "int $0x78" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x79:
			__asm__ __volatile__ ( "int $0x79" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x7A:
			__asm__ __volatile__ ( "int $0x7A" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x7B:
			__asm__ __volatile__ ( "int $0x7B" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x7C:
			__asm__ __volatile__ ( "int $0x7C" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x7D:
			__asm__ __volatile__ ( "int $0x7D" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x7E:
			__asm__ __volatile__ ( "int $0x7E" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x7F:
			__asm__ __volatile__ ( "int $0x7F" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x80:
			__asm__ __volatile__ ( "int $0x80" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x81:
			__asm__ __volatile__ ( "int $0x81" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x82:
			__asm__ __volatile__ ( "int $0x82" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x83:
			__asm__ __volatile__ ( "int $0x83" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x84:
			__asm__ __volatile__ ( "int $0x84" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x85:
			__asm__ __volatile__ ( "int $0x85" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x86:
			__asm__ __volatile__ ( "int $0x86" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x87:
			__asm__ __volatile__ ( "int $0x87" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x88:
			__asm__ __volatile__ ( "int $0x88" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x89:
			__asm__ __volatile__ ( "int $0x89" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x8A:
			__asm__ __volatile__ ( "int $0x8A" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x8B:
			__asm__ __volatile__ ( "int $0x8B" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x8C:
			__asm__ __volatile__ ( "int $0x8C" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x8D:
			__asm__ __volatile__ ( "int $0x8D" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x8E:
			__asm__ __volatile__ ( "int $0x8E" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x8F:
			__asm__ __volatile__ ( "int $0x8F" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x90:
			__asm__ __volatile__ ( "int $0x90" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x91:
			__asm__ __volatile__ ( "int $0x91" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x92:
			__asm__ __volatile__ ( "int $0x92" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x93:
			__asm__ __volatile__ ( "int $0x93" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x94:
			__asm__ __volatile__ ( "int $0x94" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x95:
			__asm__ __volatile__ ( "int $0x95" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x96:
			__asm__ __volatile__ ( "int $0x96" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x97:
			__asm__ __volatile__ ( "int $0x97" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x98:
			__asm__ __volatile__ ( "int $0x98" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x99:
			__asm__ __volatile__ ( "int $0x99" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x9A:
			__asm__ __volatile__ ( "int $0x9A" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x9B:
			__asm__ __volatile__ ( "int $0x9B" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x9C:
			__asm__ __volatile__ ( "int $0x9C" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x9D:
			__asm__ __volatile__ ( "int $0x9D" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x9E:
			__asm__ __volatile__ ( "int $0x9E" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0x9F:
			__asm__ __volatile__ ( "int $0x9F" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA0:
			__asm__ __volatile__ ( "int $0xA0" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA1:
			__asm__ __volatile__ ( "int $0xA1" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA2:
			__asm__ __volatile__ ( "int $0xA2" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA3:
			__asm__ __volatile__ ( "int $0xA3" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA4:
			__asm__ __volatile__ ( "int $0xA4" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA5:
			__asm__ __volatile__ ( "int $0xA5" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA6:
			__asm__ __volatile__ ( "int $0xA6" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA7:
			__asm__ __volatile__ ( "int $0xA7" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA8:
			__asm__ __volatile__ ( "int $0xA8" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xA9:
			__asm__ __volatile__ ( "int $0xA9" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xAA:
			__asm__ __volatile__ ( "int $0xAA" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xAB:
			__asm__ __volatile__ ( "int $0xAB" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xAC:
			__asm__ __volatile__ ( "int $0xAC" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xAD:
			__asm__ __volatile__ ( "int $0xAD" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xAE:
			__asm__ __volatile__ ( "int $0xAE" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xAF:
			__asm__ __volatile__ ( "int $0xAF" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB0:
			__asm__ __volatile__ ( "int $0xB0" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB1:
			__asm__ __volatile__ ( "int $0xB1" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB2:
			__asm__ __volatile__ ( "int $0xB2" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB3:
			__asm__ __volatile__ ( "int $0xB3" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB4:
			__asm__ __volatile__ ( "int $0xB4" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB5:
			__asm__ __volatile__ ( "int $0xB5" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB6:
			__asm__ __volatile__ ( "int $0xB6" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB7:
			__asm__ __volatile__ ( "int $0xB7" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB8:
			__asm__ __volatile__ ( "int $0xB8" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xB9:
			__asm__ __volatile__ ( "int $0xB9" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xBA:
			__asm__ __volatile__ ( "int $0xBA" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xBB:
			__asm__ __volatile__ ( "int $0xBB" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xBC:
			__asm__ __volatile__ ( "int $0xBC" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xBD:
			__asm__ __volatile__ ( "int $0xBD" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xBE:
			__asm__ __volatile__ ( "int $0xBE" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xBF:
			__asm__ __volatile__ ( "int $0xBF" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC0:
			__asm__ __volatile__ ( "int $0xC0" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC1:
			__asm__ __volatile__ ( "int $0xC1" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC2:
			__asm__ __volatile__ ( "int $0xC2" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC3:
			__asm__ __volatile__ ( "int $0xC3" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC4:
			__asm__ __volatile__ ( "int $0xC4" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC5:
			__asm__ __volatile__ ( "int $0xC5" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC6:
			__asm__ __volatile__ ( "int $0xC6" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC7:
			__asm__ __volatile__ ( "int $0xC7" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC8:
			__asm__ __volatile__ ( "int $0xC8" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xC9:
			__asm__ __volatile__ ( "int $0xC9" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xCA:
			__asm__ __volatile__ ( "int $0xCA" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xCB:
			__asm__ __volatile__ ( "int $0xCB" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xCC:
			__asm__ __volatile__ ( "int $0xCC" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xCD:
			__asm__ __volatile__ ( "int $0xCD" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xCE:
			__asm__ __volatile__ ( "int $0xCE" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xCF:
			__asm__ __volatile__ ( "int $0xCF" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD0:
			__asm__ __volatile__ ( "int $0xD0" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD1:
			__asm__ __volatile__ ( "int $0xD1" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD2:
			__asm__ __volatile__ ( "int $0xD2" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD3:
			__asm__ __volatile__ ( "int $0xD3" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD4:
			__asm__ __volatile__ ( "int $0xD4" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD5:
			__asm__ __volatile__ ( "int $0xD5" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD6:
			__asm__ __volatile__ ( "int $0xD6" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD7:
			__asm__ __volatile__ ( "int $0xD7" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD8:
			__asm__ __volatile__ ( "int $0xD8" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xD9:
			__asm__ __volatile__ ( "int $0xD9" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xDA:
			__asm__ __volatile__ ( "int $0xDA" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xDB:
			__asm__ __volatile__ ( "int $0xDB" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xDC:
			__asm__ __volatile__ ( "int $0xDC" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xDD:
			__asm__ __volatile__ ( "int $0xDD" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xDE:
			__asm__ __volatile__ ( "int $0xDE" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xDF:
			__asm__ __volatile__ ( "int $0xDF" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE0:
			__asm__ __volatile__ ( "int $0xE0" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE1:
			__asm__ __volatile__ ( "int $0xE1" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE2:
			__asm__ __volatile__ ( "int $0xE2" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE3:
			__asm__ __volatile__ ( "int $0xE3" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE4:
			__asm__ __volatile__ ( "int $0xE4" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE5:
			__asm__ __volatile__ ( "int $0xE5" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE6:
			__asm__ __volatile__ ( "int $0xE6" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE7:
			__asm__ __volatile__ ( "int $0xE7" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE8:
			__asm__ __volatile__ ( "int $0xE8" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xE9:
			__asm__ __volatile__ ( "int $0xE9" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xEA:
			__asm__ __volatile__ ( "int $0xEA" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xEB:
			__asm__ __volatile__ ( "int $0xEB" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xEC:
			__asm__ __volatile__ ( "int $0xEC" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xED:
			__asm__ __volatile__ ( "int $0xED" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xEE:
			__asm__ __volatile__ ( "int $0xEE" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xEF:
			__asm__ __volatile__ ( "int $0xEF" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF0:
			__asm__ __volatile__ ( "int $0xF0" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF1:
			__asm__ __volatile__ ( "int $0xF1" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF2:
			__asm__ __volatile__ ( "int $0xF2" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF3:
			__asm__ __volatile__ ( "int $0xF3" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF4:
			__asm__ __volatile__ ( "int $0xF4" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF5:
			__asm__ __volatile__ ( "int $0xF5" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF6:
			__asm__ __volatile__ ( "int $0xF6" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF7:
			__asm__ __volatile__ ( "int $0xF7" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF8:
			__asm__ __volatile__ ( "int $0xF8" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xF9:
			__asm__ __volatile__ ( "int $0xF9" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xFA:
			__asm__ __volatile__ ( "int $0xFA" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xFB:
			__asm__ __volatile__ ( "int $0xFB" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xFC:
			__asm__ __volatile__ ( "int $0xFC" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xFD:
			__asm__ __volatile__ ( "int $0xFD" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xFE:
			__asm__ __volatile__ ( "int $0xFE" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
		case 0xFF:
			__asm__ __volatile__ ( "int $0xFF" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
			break;
	}
}