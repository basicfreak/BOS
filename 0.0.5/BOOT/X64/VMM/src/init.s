; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                  PMM Header
;                          07/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _init

bits 64
default rel

%include 'PMM.inc'

section .init
global _init

_init:
	stc
	ret
