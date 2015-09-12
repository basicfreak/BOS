#ifndef				TASKMANAGEMENT_H_INCLUDED
	#define				TASKMANAGEMENT_H_INCLUDED

	#include <KLIB.h>

	typedef struct registers	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;    
	} __attribute__((packed)) regs; //76 Bytes

	void _TM_init(void);
	void IDT_HANDLER(regs *r);
	void killCurrentThread(regs *r);

#endif