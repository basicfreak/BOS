/*
./LIBSRC/PS2.C
IRQ12
*/

#include "MOUSE.H"
#include "8042.H"
#include "../../SYSTEM/CPU/IRQ.H"
#include <STDIO.H>

void PS2_updateCursur(void);
void MouseEvent(void);
void mouse_handler(regs *a_r);


//Mouse.inc by SANiK
//License: Use as you wish, except to cause damage
uint8_t mouse_cycle=0;     //unsigned char
int8_t mouse_byte[3];    //signed char
uint16_t SCREENSIZE_X = 80;
uint16_t SCREENSIZE_Y = 25;
uint32_t _PS2_PREX = 0;
uint32_t _PS2_PREY = 0;
uint8_t PREVCHAR;
uint8_t PREVCOLOR;

void PS2_updateCursur()
{
	unsigned int tempX = curX;
	unsigned int tempY = curY;
	movcur_disable++;
	scroll_disable++;
	
	curX = _PS2_PREX;
	curY = _PS2_PREY;
	printfc(PREVCOLOR, "%c", PREVCHAR);
	
	PREVCHAR = getCharOf((uint8_t) _PS2_X, (uint8_t) _PS2_Y);
	PREVCOLOR = getColorOf((uint8_t) _PS2_X, (uint8_t) _PS2_Y);
	
	curX = _PS2_X;
	curY = _PS2_Y;
	unsigned int tcolor = 0xF0;
	if (_PS2_0)
		tcolor -= 0x10;
	if (_PS2_1)
		tcolor -= 0x20;
	if (_PS2_2)
		tcolor -= 0x30;
	
	printfc(tcolor, "%c", PREVCHAR);
	
	curX = tempX;
	curY = tempY;
	movcur_disable--;
	scroll_disable--;
}

void MouseEvent()
{
	uint8_t tempX = (uint8_t) mouse_byte[1];
	uint8_t tempY = (uint8_t) mouse_byte[2];
	_PS2_PREX = _PS2_X;
	_PS2_PREY = _PS2_Y;
	
	if (mouse_byte[0] & 0x1)
		_PS2_0 = TRUE;
	else
		_PS2_0 = FALSE;
	if (mouse_byte[0] & 0x2)
		_PS2_1 = TRUE;
	else
		_PS2_1 = FALSE;
	if (mouse_byte[0] & 0x4)
		_PS2_2 = TRUE;
	else
		_PS2_2 = FALSE;
	
	_PS2_X += (uint32_t) ((tempX - ((mouse_byte[0] << 4) & 0x100)) / 6);
	_PS2_Y -= (uint32_t) ((tempY - ((mouse_byte[0] << 3) & 0x100)) / 6);
		
	if (_PS2_X >= SCREENSIZE_X)
		_PS2_X = (uint32_t) (SCREENSIZE_X - 1);
	if (_PS2_Y >= SCREENSIZE_Y)
		_PS2_Y = (uint32_t) (SCREENSIZE_Y - 1);
	if (_PS2_X > ROWS)
		_PS2_X = 0;
	if (_PS2_Y > COLS)
		_PS2_Y = 0;
		
	PS2_updateCursur();
}

void PS2_setScreenSize(uint16_t X, uint16_t Y)
{
	SCREENSIZE_X = X;
	SCREENSIZE_Y = Y;
}

//Mouse functions
void mouse_handler(regs *a_r) //struct regs *a_r (not used but just there)
{
	if(a_r->eax){}
	switch(mouse_cycle)
	{
		case 0:
			mouse_byte[0]=(int8_t) _8042_readPort();
			mouse_cycle++;
			break;
		case 1:
			mouse_byte[1]=(int8_t) _8042_readPort();
			mouse_cycle++;
			break;
		case 2:
			mouse_byte[2]=(int8_t) _8042_readPort();
			mouse_cycle=0;
			MouseEvent();
			break;
	}
}

void _mouse_init()
{
	_PS2_X = 0;
	_PS2_Y = 0;
	_PS2_0 = FALSE;
	_PS2_1 = FALSE;
	_PS2_2 = FALSE;
	PREVCHAR = getCharOf(0, 0);
	PREVCOLOR = getColorOf(0, 0);
	
	_8042_enablePort(2);
	_8042_writePort(2, 0xF6);
	_8042_readPort();  //Acknowledge
	_8042_writePort(2, 0xF4);
	_8042_readPort();  //Acknowledge

	//Setup the mouse handler
	install_IR(12, mouse_handler);
}