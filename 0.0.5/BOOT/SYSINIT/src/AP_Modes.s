; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0004
;                                   CPU Strap
;                          17/02/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 16

%include 'PIC.inc'

section .CPUSection
global AP_Strap

; AL = Function
; (E/R)DX = Destination
; EBX = Page Dir (Long Mode Only)

; AL
; 0 = RM to PM
; 1 = PM to RM
; 2 = RM to LM
; 3 = PM to LM
; 4 = LM to PM
; 5 = LM to RM
AP_Strap:
	cli									; These Functions CANNOT be interrupted
	cmp al, 0							; Real Mode to Protected Mode
	je short .RMtoPM
	cmp al, 1							; Protected Mode to Real Mode
	je short .PMtoRM
	cmp al, 2							; Real Mode to Long Mode
	je short .RMtoLM
	cmp al, 3							; Protected Mode to Long Mode
	je short .PMtoLM
	cmp al, 4							; Long Mode to Protected Mode
	je short .LMtoPM
	cmp al, 5 							; Long Mode to Real Mode
	je short .LMtoRM
										; If you have an invalid function,
	hlt									; we just lock up your computer ;)
	jmp $								; Have a nice and productive day.

	.RMtoPM:
		call PIC_ReMask					; Setup PIC Masks
		mov eax, cr0					; Set PM Bit in CR0
		or al, 1
		mov cr0, eax
		jmp 0x18:.inPM					; Long jump into Protected Mode
	.RMtoLM:
		call PIC_ReMask					; Setup PIC Masks
		mov cr3, ebx					; Set Page Dir (CR3)
		xchg ebx, edx					; Save Destination
		mov ecx, 0xC0000080				; Set Long Mode Bit in MSR
		rdmsr
		or ax, 0x100
		wrmsr
		xchg ebx, edx					; Restore Destination
		mov eax, cr4					; Set PSE and PAE bits in CR4
		or eax, 0x30
		mov cr4, eax
		mov eax, cr0					; Set PM and PG bits in CR0
		or eax, 0x80000001
		mov cr0, eax
		jmp 0x38:.inLM					; Long jump into Long Mode

bits 64
	.LMtoPM:
		push 0x18						; Push Protected Mode Code GDT Offset
		mov eax, .inCapMode				; Get Address of inCapMode
		push rax						; Push Address of inCapMode
		retf							; Far return to Compatibility Mode
	.LMtoRM:
		push 0x18						; Push Protected Mode Code GDT Offset
		mov eax, .inCapModeToRM			; Get Address of inCapModeToRM
		push rax						; Push Address of inCapModeToRM
		retf							; Far return to Compatibility Mode
	.inLM:
		mov ax, 0x40					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To Long Mode Data GDT Offset
		mov es, ax
		mov ss, ax
		sti								; IF = 1
		jmp rdx							; Jump to Destination

bits 32
	.PMtoRM:
		jmp 0x08:.inPRM					; Far jump to 16-Bit Protected Mode
	.PMtoLM:
		mov cr3, ebx					; Set Page Dir (CR3)
		xchg ebx, edx					; Save Destination
		mov ecx, 0xC0000080				; Set Long Mode Bit in MSR
		rdmsr
		or ax, 0x100
		wrmsr
		xchg ebx, edx					; Restore Destination
		mov eax, cr4					; Set PSE and PAE bits in CR4
		or eax, 0x30
		mov cr4, eax
		mov eax, cr0					; Set PM and PG bits in CR0
		or eax, 0x80000001
		mov cr0, eax
		jmp 0x38:.inLM
	.inPM:
		mov ax, 0x20					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To Protected Mode Data DST Offset
		mov es, ax
		mov ss, ax
		sti								; IF = 1
		jmp edx							; Jump to Destination
	.inCapMode:
		mov ax, 0x20					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To Protected Mode Data DST Offset
		mov es, ax
		mov ss, ax
		xchg ebx, edx					; Save Destination
		mov ecx, 0xC0000080				; Unset Long Mode Bit in MSR
		rdmsr
		and ax, 0xFEFF
		wrmsr
		xchg ebx, edx					; Restore Destination
		mov eax, cr0					; Clear PG bit in CR0
		and eax, 0x7FFFFFFF
		mov cr0, eax
		xor eax, eax					; Clear CR3 (Page Dir)
		mov cr3, eax
		jmp edx							; Jump to Destination
	.inCapModeToRM:
		mov ax, 0x20					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To Protected Mode Data DST Offset
		mov es, ax
		mov ss, ax
		xchg ebx, edx					; Save Destination
		mov ecx, 0xC0000080				; Unset Long Mode Bit in MSR
		rdmsr
		and ax, 0xFEFF
		wrmsr
		xchg ebx, edx					; Restore Destination
		mov eax, cr0					; Clear PG bit in CR0
		and eax, 0x7FFFFFFF
		mov cr0, eax
		xor eax, eax					; Clear CR3 (Page Dir)
		mov cr3, eax
		jmp 0x08:.inPRM					; Far jump to 16-Bit Protected Mode

bits 16
	.inPRM:
		mov ax, 0x10					; Set Data, Extra, and Stack Segments
		mov ds, ax						; To 16-Bit Protected Mode Data Offset
		mov es, ax
		mov ss, ax
		mov eax, cr0					; Clear PE and PG bit in CR0
		and eax, 0x7FFFFFFE
		mov cr0, eax
		jmp 0:.inRM						; Far jump to Real Mode
	.inRM:
		xor ax, ax						; Set Data, Extra, and Stack Segments
		mov ds, ax						; To Null GDT Segment
		mov es, ax
		mov ss, ax
		call PIC_ReMask					; Setup PIC Masks
		sti								; IF = 1
		jmp DWORD edx					; Far jump to Destination
