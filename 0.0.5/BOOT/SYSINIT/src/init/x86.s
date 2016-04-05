; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                           System Initialization x86
;                          04/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

bits 32

global init_x86
extern ERROR

%include 'linker.inc'

init_x86:
	ret
