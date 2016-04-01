; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                     CPUID
;                          01/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

[bits 16]

global INIT_CPUID
global BSP.Vendor
global BSP.Info
global BSP.Features
global BSP.ExtendedFeatures
global BSP.ExtendedInfo

ID_FLAG equ 0x00200000

INIT_CPUID:
	.DoesCPUIDExist:
		pushfd								;Save EFLAGS
	    pushfd								;Store EFLAGS
	    pop eax
	    xor eax, ID_FLAG					;Invert the ID bit in stored EFLAGS
	    push eax
	    popfd								;Load stored EFLAGS (with ID bit inverted)
	    pushfd								;Store EFLAGS again (ID bit may or may not be inverted)
	    pop eax								;eax = modified EFLAGS (ID bit may or may not be inverted)
	    pop ebx
	    push ebx
	    xor eax,ebx							;eax = whichever bits were changed
	    popfd								;Restore original EFLAGS
	    test eax, ID_FLAG					;eax = zero if ID bit can't be changed, else non-zero
	    jnz .GetVendor
	    stc
	    ret
	.GetVendor:
		xor eax, eax
		cpuid
		push eax
		mov [BSP.Vendor], ebx
		mov [BSP.Vendor + 4], edx
		mov [BSP.Vendor + 8], ecx
	pop eax
	push eax
	cmp eax, 1
	jb .ExtendedCPUID
	.GetFeatures:
		mov eax, 1
		cpuid
		mov [BSP.Info], eax
		mov [BSP.Features], edx
		mov [BSP.Features + 4], ecx
	pop eax
	cmp eax, 7
	jb .ExtendedCPUID
	.GetExtFeatures:
		mov eax, 7
		xor ecx, ecx
		cpuid
		mov [BSP.ExtendedFeatures], ebx
		mov [BSP.ExtendedFeatures + 4], ecx
	.ExtendedCPUID:
		mov eax, 0x80000000
		cpuid
		cmp eax, 0x80000001
		jb .Return
		mov eax, 0x80000001
		cpuid
		mov [BSP.ExtendedInfo], edx
		mov [BSP.ExtendedInfo + 4], ecx
	.Return:
		clc
		ret

section .data

ALIGN 0x10

BSP:
	.Vendor db "            "
	.Info dd 0
	.Features dq 0
	.ExtendedFeatures dq 0
	.ExtendedInfo dq 0
