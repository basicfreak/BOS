#ifndef				_FDC_H_INCLUDED
	#define				_FDC_H_INCLUDED
	
	#include <api.h>

	#define FDC_PRI_BASE 0x3F0
	#define FDC_SEC_BASE 0x370

	#define FDC_STRA_OFF 0
	#define FDC_STRB_OFF 1
	#define FDC_DOR_OFF 2
	#define FDC_TDR_OFF 3
	#define FDC_MSR_OFF 4
	#define FDC_DSR_OFF 4
	#define FDC_FIFO_OFF 5
	#define FDC_DIR_OFF 7
	#define FDC_CCR_OFF 7

#endif