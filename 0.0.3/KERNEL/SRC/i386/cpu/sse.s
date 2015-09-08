;SSE.s

[global _SSE_init]
[global SSE_Save]
[global SSE_Load]

;void _SSE_init(void)
_SSE_init:
	pusha
	mov eax, 0x1
	cpuid
	test edx, 1<<25
	jz .noSSE

	mov eax, cr0
	and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
	or ax, 0x2			;set coprocessor monitoring  CR0.MP
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4, eax

	popa
	ret
	.noSSE:
		hlt
		jmp .noSSE

;void SSE_Save(void* ssedLocation)
SSE_Save:
	push ebp             ; create stack frame
	mov ebp, esp
	fxsave [ebp+8]    ; grab the first argument
	pop ebp             ; restore the base pointer
	ret

;void SSE_Load(void* ssedLocation)
SSE_Load:
	push ebp             ; create stack frame
	mov ebp, esp
	fxrstor [ebp+8]    ; grab the first argument
	pop ebp             ; restore the base pointer
	ret