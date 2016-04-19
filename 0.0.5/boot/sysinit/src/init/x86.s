; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                           System Initialization x86
;                          04/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

; I'm actually unsure how I will handle a 32-bit processor ATM...
; This file will likely be modified in the future.

bits 32

global init_x86
extern ERROR

%include 'linker.inc'
%include 'term.inc'
%include 'data/rodata.inc'
%include 'CPUID.inc'

init_x86:
	mov si, MSG.PDIR
	call puts32

	mov edi, 0x200000					; 0 Out Temporary Page Directory Memory
	mov ecx, 0x600
	xor eax, eax
	rep stosd
	or al, 3
	mov edi, 0x201000
	mov ecx, 0x400
	.PTLoop:
		stosd
		add eax, 0x1000
		loop .PTLoop
	mov edi, 0x200000
	mov eax, 0x200003
	mov [edi + 0xFFC], eax
	add eax, 0x1000
	mov [edi], eax
	mov [edi + 0x600], eax
	add eax, 0x1000
	mov [edi + 4], eax
	mov [edi + 0x604], eax

	mov eax, 0x200000
	mov cr3, eax
	mov eax, cr0
	bts eax, 31
	mov cr0, eax

	mov si, MSG.Done
	call puts32
	mov si, MSG.PMM
	call puts32

	mov esi, Files.PM_PMM				; Load Physical Memory Manager
	mov edi, 0x100000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts32
	mov si, MSG.VMM
	call puts32

	mov esi, Files.PM_VMM				; Load Virtual Memory Manager
	mov edi, 0x108000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts32
	mov si, MSG.Linker
	call puts32

	mov esi, Files.PM_Link				; Load BOS Linker / Builder
	mov edi, 0x110000
	call FILE_IO_WRAP
	jc .Error

	mov si, MSG.Done
	call puts32

	mov eax, 0xC0110000					; Call BOS Linker / Builder
	mov ebx, BSP.Vendor
	call eax
	ret									; Return if we get back here...

	.Error:
		mov si, MSG.Fail
		call puts32
		ret

FILE_IO_WRAP:
	xor eax, eax						; FILE_IO (Open)
	call FILE_IO
	jc .NotFound						; If Error, Not Found
	mov eax, 1							; FILE_IO (Read)
	call FILE_IO
	jc .NotFound						; If Error, Not Found
	push eax							; Save EAX
	mov eax, 3							; FILE_IO (Close)
	call FILE_IO
	pop eax								; Restore EAX
	ret									; Return
	.NotFound:
		stc								; CF = 1 on Error
		ret								; Return
