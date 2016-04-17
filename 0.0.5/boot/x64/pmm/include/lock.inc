; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                PMM Lock Header
;                          06/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

extern _Lock
extern _Unlock
extern Total_RAM
extern Used_RAM
extern Free_RAM
extern PMM_Entries

PMM_STACK		equ 0xFFFFFFFFC0300000
