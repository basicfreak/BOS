#ifndef _SYSTEMCALLS_H_INCLUDE
	#define _SYSTEMCALLS_H_INCLUDE

	#define _VMM_map(v, p, w) __asm__ __volatile__("int $0xF2" : : "a" (0x80), "d" (v), "c" (p), "b" (w))
	#define _VMM_umap(v) __asm__ __volatile__("int $0xF2" : : "a" (0x81), "d" (v))
	#define _IOPERM() __asm__ __volatile__ ( "int $0xF1" : : "a" (0x81))
	#define _VMM_newDIR(x) __asm__ __volatile__ ( "int $0xF2" : "=a" (x) : "a" (0xF0))
	#define _VMM_allocOther(d, v, w) __asm__ __volatile__ ( "int $0xF2" : : "a" (((unsigned)d & 0xFFFFF000) | 0xF2), "d" (v), "b" (w))
	#define _TM_EXEC(p, i, n) __asm__ __volatile__ ("int $0xF1" : : "a" (0x02), "d" (p), "b" (i), "S" (n))
	#define _TM_FORK(i) __asm__ __volatile__ ("int $0xF1" : : "a" (0x01), "d" (i))
	#define _IPC_WAITINT(i) __asm__ __volatile__ ("int $0xF0" : : "a" (0x80), "b" (i))
	#define _VMM_getPageOther(d, v, ret) __asm__ __volatile__ ("int $0xF2" : "=a" (ret) : "a" (((unsigned)d & 0xFFFFF000) | 0xFF), "d" (v))
	#define _yeild() __asm__ __volatile__ ("int $0xF1" : : "a" (0x03))

#endif