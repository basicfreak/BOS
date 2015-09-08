/*
./DRIVERSRC/SYSTEM/API/API.C
*/

#include "API.H"
#include "../CPU/INT.H"
#include "THREADMAN.H"
#include "../FS/VFS.H"
#include <STDIO.H>

void _API_INT_30(regs *r);
void _API_INT_31(regs *r);

/* INT 0x30: Standard Inputs
AL = Command (X | X | X | X | X | Mouse | gets | getch)
*/
void _API_INT_30(regs *r)
{
	printf("INT 0x30 Has Been Called ;)\nEAX = 0x%x\tEBX = 0x%x\tECX = 0x%x\tEDX = 0x%x\nESI = 0x%x\tEDI = 0x%x\n", r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi);
}


/* INT 0x31: Standard Video Outputs
REGS:
AL = Command (X | X | X | PUTS | SCROLLDIS | POS | MOVECURDIS | CLS)
AH = Color (0xBF)
BL = X pos
BH = Y pos
esi = string pointer
*/
void _API_INT_31(regs *r)
{
	unsigned int Xt = curX;
	unsigned int Yt = curY;
	uint8_t command = (r->eax & 0xFF);
	unsigned int Color = (unsigned int) ((r->eax >> 8) & 0xFF);
	char* StringTemp = (char*) r->esi;
	unsigned int backupColor = getColor();
	unsigned int X = (unsigned int) (r->ebx & 0xFF);
	unsigned int Y = (unsigned int) (r->ebx >> 8) & 0xFF;
	setColor(Color);
	if (command & 0x01)
		cls();
	if (command & 0x02)
		movcur_disable++;
	if (command & 0x04)
		setPos(X, Y);
	if (command & 0x08)
		scroll_disable++;
	if (command & 0x10)
		puts(StringTemp);
	if (command & 0x20)
		putch((char) r->esi);
	// if we need a clean-up...
	if (command & 0x02)
		movcur_disable--;
	if (command & 0x08)
		scroll_disable--;
	if (command & 0x04)
		setPos(Xt, Yt);
	setColor(backupColor);
}

void _API_init()
{
	install_INT(0, _API_INT_30);
	install_INT(1, _API_INT_31);
	
}

void INT(uint8_t num, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
	if (num < 0x30)
		return;
	if (num > 0x4F)
		return;		// Error we have no INT this high
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
		default:
			printf("error\n");
	}
	
	
}