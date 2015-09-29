void putch(const char out);
void puts(const char* out);

void putch(const char out)
{
	__asm__ __volatile__("outb %0, %1" : : "a"(out), "Nd"(0xE9));
}

void puts(const char* out)
{
	while(*out) {
		__asm__ __volatile__("outb %0, %1" : : "a"(*out++), "Nd"(0xE9));
	}
}
