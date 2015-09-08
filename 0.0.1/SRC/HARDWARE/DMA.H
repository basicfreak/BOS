/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/DMA.H					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <FORMATTING.H>

#ifndef				DMA_H_INCLUDED
	#define				DMA_H_INCLUDED
	
	error _DMA_Setup(uint8_t channel, bool write, uint32_t addr, uint16_t count);
/*
	void _DMA_write(void);
	void _DMA_read(void);
*/
#endif