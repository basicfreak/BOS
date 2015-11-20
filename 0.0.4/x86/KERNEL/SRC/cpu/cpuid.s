[global _CPUID_init]
[global CPUFLAGS]
[global CPUFLAGS.Standard]
[global CPUFLAGS.Extended]
[global CPU_VENDOR_STRING]




SECTION .text ; Code area, Only Executable code please
align 0x00001000

[bits 32]

_CPUID_init:
xchg bx, bx
	xor eax, eax
	cpuid
	mov DWORD [CPU_VENDOR_STRING], ebx
	mov DWORD [CPU_VENDOR_STRING + 4], ecx
	mov DWORD [CPU_VENDOR_STRING + 8], edx
	mov eax, 1
	cpuid
	mov DWORD [CPUFLAGS.Standard], edx
	mov DWORD [CPUFLAGS.Extended], ecx
	ret




SECTION .data
align 4

CPUFLAGS:
	.Standard dd 0
	.Extended dd 0

CPU_VENDOR_STRING dd 0, 0, 0
db 0




SECTION .rodata
align 0x00001000

CPU_Vendor_ID:
	.OldAMD       db "AMDisbetter!", 0
	.AMD          db "AuthenticAMD", 0
	.INTEL        db "GenuineIntel", 0
	.VIA          db "CentaurHauls", 0
	.OldTRANSMETA db "TransmetaCPU", 0
	.TRANSMETA    db "GenuineTMx86", 0
	.CYRIX        db "CyrixInstead", 0
	.CENTAUR      db "CentaurHauls", 0
	.NEXGEN       db "NexGenDriven", 0
	.UMC          db "UMC UMC UMC ", 0
	.SIS          db "SiS SiS SiS ", 0
	.NSC          db "Geode by NSC", 0
	.RISE         db "RiseRiseRise", 0

CPU_Vendor_Name:
	.AMD          db "AMD", 0
	.INTEL        db "INTEL", 0
	.VIA          db "VIA", 0
	.TRANSMETA    db "TRANSMETA", 0
	.CYRIX        db "CYRIX", 0
	.NEXGEN       db "NEXGEN", 0
	.UMC          db "UMC", 0
	.SIS          db "SIS", 0
	.NSC          db "NSC", 0
	.RISE         db "RISE", 0

INTEL_CPU_Brand_Name:
	.C db "Celeron", 0
	.CM db "Mobile Celeron", 0
	.P3 db "Pentium III", 0
	.P3X db "Pentium III Xeon", 0
	.P3M db "Mobile Pentium III", 0
	.P4 db "Pentium 4", 0
	.P4M db "Mobile Pentium 4", 0
	.G db "Genuine", 0
	.X db "Xeon", 0
