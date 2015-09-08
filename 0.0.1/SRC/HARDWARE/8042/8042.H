/*
./DRIVERSRC/8042/8042.H
*/

#ifndef				_8042_H_INCLUDED
	#define				_8042_H_INCLUDED
	
	#include <formatting.h>
	
	uint8_t _8042_readPort(void);
	void _8042_writePort(int port, uint8_t data);
	bool _8042_disablePort(int port);
	bool _8042_enablePort(int port);
	bool _8042_init(void);
	
#endif