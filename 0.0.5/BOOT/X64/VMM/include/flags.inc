; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                   VMM FLAGS
;                          07/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

_PRESENT			equ 1
_WRITE				equ 2
_USER				equ 4
_WRITETHROUGH		equ 8
_CACHEDISABLE		equ 0x10
_ACCESSED			equ 0x20
_DIRTY				equ 0x40
_SIZE				equ 0x80
_GLOBAL				equ 0x100

_PML4				equ 0xFFFFFF7FBFDFE000
_PDP				equ 0xFFFFFF7FBFC00000
_PD					equ 0xFFFFFF7F80000000
_PT					equ 0xFFFFFF0000000000
