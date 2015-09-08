/*
./SRC/INCLUDE/API.h
*/

#ifndef				API_H_INCLUDED
	#define				API_H_INCLUDED

	// Define basic data types
	typedef		char					bool;
	typedef 	int						size_t;
	typedef 	signed char				int8_t;
	typedef 	unsigned char			uint8_t;
	typedef 	short					int16_t;
	typedef 	unsigned short			uint16_t;
	typedef 	int 					int32_t;
	typedef 	unsigned				uint32_t;
	typedef 	long long				int64_t;
	typedef 	unsigned long long		uint64_t;

	#define		TRUE					1
	#define		FALSE					0
	#define		true					1
	#define		false					0
	#define DEBUG
	#define DEBUG_FULL

	typedef struct registers	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;    
	} regs;

	extern uint32_t _API_Get_Symbol(const char* Name);
	extern void outb(uint16_t _port, uint8_t _data);
	extern uint8_t inb(uint16_t _port);
	#ifdef DEBUG
		extern void DEBUG_printf(const char *String, ...);
	#endif
	extern void outw(uint16_t port, uint16_t data);
	extern uint16_t inw(uint16_t port);
	extern void outl(uint16_t port, uint32_t data);
	extern uint32_t inl(uint16_t port);
	extern size_t strlen(const uint8_t *str);
	extern bool streql( const uint8_t *s1, const uint8_t *s2 );
	extern int32_t strcmp( const uint8_t * s1, const uint8_t * s2 );
	extern uint8_t strcpy(uint8_t *s1, const uint8_t *s2);
	extern void newHandler(void (*IR)(regs *r), uint8_t idtno);
	extern void *malloc(uint32_t);
	extern void *calloc(uint32_t);
	extern void _VMM_map(uint32_t, uint32_t, bool, bool);
	extern void *_VMM_unmap(uint32_t);
	extern void free(void*, uint32_t);
	extern bool _API_Add_Symbol(const char* Name, uint32_t Funct);
	extern void INT(uint8_t num, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
	extern void *memcpy (void *dest, const void *src, size_t count);
	extern void *memset (void *dest, uint8_t val, size_t count);
	extern uint16_t *memsetw (uint16_t *dest, uint16_t val, size_t count);
	extern uint32_t *memsetl (uint32_t *dest, uint32_t val, size_t count);
	extern uint32_t Load_ELF(void *File);
	extern bool Load_Mod(void* ELF_Loc);
	extern void _8259_Enable_IRQ(uint8_t IRQ);

	extern void PIT_wait(uint32_t T);
	extern uint32_t PIT_Ticks(void);
	extern void sleep(uint32_t S);

	int init(void);

#endif