typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned uint32_t;

#define TRUE 1
#define FALSE 0

int init(void);

void DEBUG_print(const char* Str);
void DEBUG_putch(const char Chr);

extern void outb(uint16_t, uint8_t);

void NewThreadAA(void);

int init()
{
	// for(char x = 0; x < 4; x++)
		__asm__ __volatile__ ( "int $0xF1" : : "a" (0x01), "d" ((uint32_t)(&NewThreadAA))); // Fork Thread

	__asm__ __volatile__ ( "int $0xF1" : : "a" (0x81)); // Request I/O Permission
	while(TRUE) {
		DEBUG_print("Test Of First Binary...\n");
		__asm__ __volatile__ ( "int $0xF1" : : "a" (0x03)); // Yeild
	}
	return FALSE;
}

void DEBUG_print(const char* Str)
{
	while(*Str)
		DEBUG_putch(*Str++);
}

void DEBUG_putch(const char Chr)
{
	outb(0xE9, (uint8_t) Chr);
}

void NewThreadAA()
{
	// __asm__ __volatile__ ( "int $0xF1" : : "a" (0x81)); // Request I/O Permission
	// __asm__ __volatile__ ("xchg %bx, %bx");
	while(TRUE) {
		DEBUG_print("Test Of Fork...\n");
		__asm__ __volatile__ ( "int $0xF1" : : "a" (0x03)); // Yeild
	}
}
