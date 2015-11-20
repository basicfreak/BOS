%include "./include/cpu/cpuid.inc"
%include "./include/mem/phys.inc"
%include "./include/cpu/apic.inc"

[global kmain]




SECTION .text ; Code area, Only Executable code please
align 0x00001000

[bits 32]
kmain:
	call _CPUID_init
	call _PMM_init
	call _APIC_init
;	call _VMM_init
;	call _CPU_init
;	call _TM_init
;	mov DWORD [TSSBase.esp0], esp
;	sti
xchg bx, bx
	.idleLoop:
		hlt
		jmp .idleLoop
