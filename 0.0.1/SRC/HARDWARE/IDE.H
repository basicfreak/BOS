/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/IDE.H					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef IDE_H_INCLUDED
	#define IDE_H_INCLUDED

	#include <FORMATTING.H>

	void _IDE_init(void);
	error _IDE_IO(uint8_t drive, bool write, uint64_t start, uint16_t count, void* Buffer);
	void _IDE_IN(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count);
	void _IDE_OUT(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count);

#endif