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

	mov eax, 0x110000					; Call BOS Linker / Builder
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
