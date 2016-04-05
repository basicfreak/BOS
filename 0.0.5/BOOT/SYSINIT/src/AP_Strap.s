; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   CPU Strap
;                          05/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; NOTE: The first time you enter Long Mode, all other modes except for Real Mode
; will actually be Capability Mode!

bits 16
section .CPUSection
global AP_Strap
extern gdt

%include 'PIC.inc'

; AL = Function
; (E/R)DX = Destination
; EBX = Page Dir

; Functions (AL)
; 0 = RM to PM 32-Bit
; 1 = PM 32-Bit to RM
; 2 = RM to LM
; 3 = PM 32-Bit to LM
; 4 = LM to CM32
; 5 = LM to RM
; 6 = LM to CM16
; 7 = PM 32-Bit to PM 16-Bit
; 8 = PM 16-Bit to PM 32-Bit

AP_Strap:
	cli									; These Functions CANNOT be interrupted
	cmp al, 0							; Real Mode to Protected Mode 32
	je short .RMtoPM
	cmp al, 1							; Protected Mode 32 to Real Mode
	je short .PMtoRM
	cmp al, 2							; Real Mode to Long Mode
	je short .RMtoLM
	cmp al, 3							; Protected Mode 32 to Long Mode
	je short .PMtoLM
	cmp al, 4							; Long Mode to Capability Mode 32-Bit
	je short .LMtoCM32
	cmp al, 5							; Long Mode to Real Mode
	je short .LMtoRM
	cmp al, 6							; Long Mode to Capability Mode 16-Bit
	je short .LMtoCM16
	cmp al, 7							; Protected Mode 32 to Protected Mode 16
	je short .PMtoPM16
	cmp al, 8							; Protected Mode 16 to Protected Mode 32
	je short .PM16toPM
	.lockup:							; If you have an invalid function,
		hlt								; we just lock up your computer ;)
		jmp .lockup						; Have a nice and productive day.

	.RMtoPM:
		test ebx, ebx
		jnz .RMtoPM_Paging
		jmp .RMtoPM_NoPageing
	.PM16toPM:
		jmp 0x18:.inPM					; Long Jump to 32-Bit Protected Mode
	.RMtoLM:
		call PIC_ReMask					; Setup PIC Masks
		mov eax, cr0					; Set PM Bit in CR0
		or al, 1
		mov cr0, eax
		jmp 0x18:.PMtoLM				; Long Jump to 32-Bit Protected Mode

bits 32
	.PMtoRM:
		jmp 0x08:.inPRM					; Long Jump to 16-Bit Protected Mode
	.PMtoLM:
		mov cr3, ebx					; Set Page Directory (CR3)
		xchg ebx, edx					; Save Destination
		mov ecx, 0xC0000080				; Set Long Mode Bit in MSR
		rdmsr
		bts eax, 8
		wrmsr
		xchg ebx, edx					; Restore Destination
		mov eax, cr4					; Set PSE and PAE Bits in CR4
		or eax, 0x30
		mov cr4, eax
		mov eax, cr0					; Set PG Bit in CR0
		bts eax, 31
		mov cr0, eax
		jmp 0x38:.inLM					; Long Jump to Long Mode
	.PMtoPM16:
		jmp 0x08:.inCM16				; Long Jump to 16-Bit Protected Mode

bits 64
	.LMtoCM32:
		push 0x18						; Push 32-Bit Protected Mode GDT Code
		mov eax, .inPM					; Get Address of .inPM
		push rax						; Push Address
		o64 retf						; Far Return to 32-Bit Capability Mode
	.LMtoCM16:
		push 0x08						; Push 16-Bit Protected Mode GDT Code
		mov eax, .inCM16				; Get Address of .inCM16
		push rax						; Push Address
		o64 retf						; Far Return to 16-Bit Capability Mode
	.LMtoRM:
		push 0x08						; Push 16-Bit Protected Mode GDT Code
		mov eax, .inPRM					; Get Address of .inPRM
		push rax						; Push Address
		o64 retf						; Far Return to 16-Bit Capability Mode
	.inLM:
		mov ax, 0x40					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To Long Mode GDT Data
		mov es, ax
		mov ss, ax
		sti								; Enable Interrupts
		jmp rdx							; Jump to Destination

bits 32
	.inPM:
		mov ax, 0x20					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To 32-Bit Protected Mode GDT Data
		mov es, ax
		mov ss, ax
		sti								; Enable Interrupts
		jmp edx							; Jump to Destination

bits 16
	.inCM16:
		mov ax, 0x10					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To 16-Bit Protected Mode GDT Data
		mov es, ax
		mov ss, ax
		sti								; Enable Interrupts
		jmp DWORD edx					; Far Jump to Destination (I think...)
	.inPRM:
		mov ax, 0x10					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To 16-Bit Protected Mode GDT Data
		mov es, ax
		mov ss, ax
		mov eax, cr0					; Clear PG and PM Bits CR0
		and eax, 0x7FFFFFFE
		mov cr0, eax
		jmp 0:.inRM						; Far Jump into Real Mode
	.inRM:
		xor ax, ax						; Set Data, Extra, and Stack Segments
		mov ds, ax						; To NULL (16-bit Real Mode Data)
		mov es, ax
		mov ss, ax
		call PIC_ReMask					; Setup PIC Masks
		sti								; Enable Interrupts
		jmp DWORD edx					; Far Jump to Destination
	.RMtoPM_Paging:
		call PIC_ReMask					; Setup PIC Masks
		mov cr3, ebx					; Set Page Directory (CR3)
		mov eax, cr0					; Set PM and PG Bit in CR0
		or eax, 0x80000001
		mov cr0, eax
		jmp 0x18:.inPM					; Far Jump to Protected Mode 32
	.RMtoPM_NoPageing:
		call PIC_ReMask					; Setup PIC Masks
		mov eax, cr0					; Set PM Bit in CR0
		or al, 1
		mov cr0, eax
		jmp 0x18:.inPM					; Far Jump to Protected Mode 32
