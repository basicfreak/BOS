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
		pushfd							; Save Original EFLAGS
	    pushfd							; Save Original EFLAGS
	    pop eax							; EAX = Original EFLAGS
	    xor eax, ID_FLAG				; Invert the ID bit in stored EFLAGS
	    push eax						; Save Modified EFLAGS
	    popfd							; Restore Modified EFLAGS
	    pushfd							; Save New EFLAGS
	    pop eax							; Get New EFLAGS
	    pop ebx							; Get Modified EFLAGS
	    push ebx						; Save Modified EFLAGS
	    xor eax,ebx						; EAX = New EFLAGS XOR Modified EFLAGS
	    popfd							; Restore original EFLAGS
	    test eax, ID_FLAG				; IF EAX != 0 Continue
	    jnz .GetVendor
	    stc								; Else Error (set CF)
	    ret								; Return
	.GetVendor:
		xor eax, eax					; CPUID, EAX = 0
		cpuid
		push eax						; Save CPUID Max Standard ID
		mov [BSP.Vendor], ebx			; Save Vendor String
		mov [BSP.Vendor + 4], edx
		mov [BSP.Vendor + 8], ecx
	pop eax
	push eax
	cmp eax, 1							; Is CPUID Max Standard ID >= 1?
	jb .ExtendedCPUID					; If Not, Skip this
	.GetFeatures:
		mov eax, 1						; CPUID, EAX = 1
		cpuid
		mov [BSP.Info], eax				; Save CPU Info
		mov [BSP.Features], edx			; Save CPU Features
		mov [BSP.Features + 4], ecx
	pop eax
	cmp eax, 7							; Is CPUID Max Standard ID >= 7
	jb .ExtendedCPUID					; If Not, Skip this
	.GetExtFeatures:
		mov eax, 7						; CPUID, EAX = 7, ECX = 0
		xor ecx, ecx
		cpuid
		mov [BSP.ExtendedFeatures], ebx	; Save Extended Features
		mov [BSP.ExtendedFeatures + 4], ecx
	.ExtendedCPUID:
		mov eax, 0x80000000				; Get CPUID Max Extended ID
		cpuid							; CPUID, EAX = 0x80000000
		cmp eax, 0x80000001				; Is CPUID Max Extended ID >= 0x80000001
		jb .Return						; If not, Skip this
		mov eax, 0x80000001				; CPUID, EAX = 0x80000001
		cpuid
		mov [BSP.ExtendedInfo], edx		; Save Extended Info
		mov [BSP.ExtendedInfo + 4], ecx
	.Return:
		clc								; Make sure CF = 0
		ret								; Return

section .data

ALIGN 0x10

BSP:
	.Vendor db "            "
	.Info dd 0
	.Features dq 0
	.ExtendedFeatures dq 0
	.ExtendedInfo dq 0
