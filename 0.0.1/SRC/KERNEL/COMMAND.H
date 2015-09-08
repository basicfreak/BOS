/*
./SRC/COMMAND.H
*/

#include <FORMATTING.H>

#ifndef				COMMAND_H_INCLUDED
	#define				COMMAND_H_INCLUDED
	
	bool CMD_ACTIVE;
	
	void cmd_handler(void);
	void init_cmd(void);
	
#endif