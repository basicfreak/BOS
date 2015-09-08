/*
./INCLUDE/PS2.H
*/

#ifndef				PS2_H_INCLUDED
	#define				PS2_H_INCLUDED

	#include <FORMATTING.H>
	
	uint32_t		_PS2_X;
	uint32_t		_PS2_Y;
	bool		_PS2_0;
	bool		_PS2_1;
	bool		_PS2_2;
	
	enum PS2_Commands
	{
		/*
		0xFF	 Reset
		0xFE	 Resend
		0xF6	 Set Defaults
		0xF5	 Disable Data Reporting
		0xF4	 Enable Data Reporting
		0xF3	 Set Sample Rate
		0xF2	 Get Device ID
		0xF0	 Set Remote Mode
		0xEE	 Set Wrap Mode
		0xEC	 Reset Wrap Mode
		0xEB	 Read Data
		0xEA	 Set Stream Mode
		0xE9	 Status Request
		*/
		_PS2_RESET		=	0xFF,
		_PS2_RESEND		=	0xFE,
		_PS2_DEFAULT	=	0xF6,
		_PS2_DISABLEDATA=	0xF5,
		_PS2_ENABLEDATA	=	0xF4,
		_PS2_SETRATE	=	0xF3,
		_PS2_GEDID		=	0xF2,
		_PS2_SETREMOTE	=	0xF0,
		_PS2_SETWARP	=	0xEE,
		_PS2_RESERWARP	=	0xEC,
		_PS2_READ		=	0xEB,
		_PS2_SETSTREAM	=	0xEA,
		_PS2_STATUS		=	0xE9
	};
	
	void _mouse_init(void);
	void PS2_setScreenSize(uint16_t X, uint16_t Y);

#endif