/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/FDC.H					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FDC_H_INCLUDED
	#define FDC_H_INCLUDED

	#include <FORMATTING.H>

	void _FDC_init(void);
	error _FDC_IO(bool write, int start, uint8_t count, void *buffer);
	void _FDC_IN(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count);
	void _FDC_OUT(uint8_t LOCALID, error *ERRORCODE, void *Buffer, uint64_t Start, uint16_t count);

#endif