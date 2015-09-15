#ifndef _IO_H_INCLUDE
	#define _IO_H_INCLUDE

	#define outb(p, v) __asm__ __volatile__("outb %0, %1" : : "a"(v), "Nd"(p))
	#define inb(p, v) __asm__ __volatile__("inb %0, %1" : "=a"(v) : "Nd"(p))
	#define outw(p, v) __asm__ __volatile__ ("outw %1, %0" : : "dN" (p), "aN" (v))
	#define inw(p, v) __asm__ __volatile__ ("inw %1, %0" : "=aN" (v) : "dN" (p))
	#define outl(p, v) __asm__ __volatile__ ("outl %1, %0" : : "dN" (p), "aL" (v))
	#define inl(p, v) __asm__ __volatile__ ("inl %1, %0" : "=aL" (v) : "dN" (p))

	#define memcpy(d, s, l) __asm__ __volatile__ ("rep movsb" :  : "S" (s), "D" (d), "c" (l))
	#define memcpyw(d, s, l) __asm__ __volatile__ ("rep movsw" :  : "S" (s), "D" (d), "c" (l))
	#define memcpyd(d, s, l) __asm__ __volatile__ ("rep movsd" :  : "S" (s), "D" (d), "c" (l))

#endif