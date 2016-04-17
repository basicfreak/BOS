; -------------------------------------- --------------------------------------
;                                   BOS 0.0.5
;                                  BUILD: 0005
;                                    VMM MAP
;                          12/04/2016 - Brian T Hoover
; -----------------------------------------------------------------------------

global _VMM_map

bits 64
default rel

%include 'flags.inc'
%include 'RefTable.inc'
%include 'PMM.inc'

; RDX = Virtual Address To Map to
; RAX = Physical Address if page aligned
;   else (0xFF0 = >4GB, 0xFF1 = >16MB < 4GB, 0xFF2 = >1MB <16MB, 0xFF3 = < 1MB)
; RBX = Flags (1 = write, 2 = user, 4 = exec, 8 = reserved Memory - no Reference)
_VMM_map:
	push rax							; Save Modified Registers
	push rcx
	push rdx
	push rdi
	push r15
	push r10

	test rdx, 0xFFF						; Check for page alignment on VADDR
	jnz .Error
	test rdx, rdx						; Make sure we are not mapping to NULL
	jz .Error

	test rax, 0xFFF						; If PADDR is aligned, it was provided
	jz .HavePhysical
	and rax, 3							; Else AND PADDR, 3
	mov rcx, 0x1000						; One Page (4KB)
	call _PMM_alloc						; Allocate
	jc .Error							; If CF Error
  .HavePhysical:
	mov r15, rax						; Save Physical Address to r15
	mov r10, rdx						; Save Virtual Address to r10

	bt rbx, 3							; Is this Reserved Memory?
	jc .NoReferenceUpdate				; Yes, Don't update reference table
	call _VMM_RefInc					; else, Update reference table
  .NoReferenceUpdate:
	mov rdx, r10						; Calculate Offset into PML4
	shr rdx, 36
	and rdx, 0xFF8
	mov rdi, _PML4						; PML4 Address
	add rdi, rdx						;    += Offset

	mov rax, r10						; Calculate Offset into PDP
	shr rax, 27
	and rax, 0x1FFFF8
	mov rdx, _PDP						; PDP Address
	add rdx, rax						;    += Offset

	bt QWORD [rdi], 0					; Does PDP Exist?
	jc .PD								; Yes, goto .PD
	xor rax, rax
	mov rcx, 0x1000
	call _PMM_alloc						; Allocate a page for PDP
	jc .Error
	call _VMM_RefInc					; Increase Reference
	or rax, (_GLOBAL | _WRITE | _PRESENT)
	stosq								; Map PDP
	invlpg [rdx]						; Invalidate Recursive Address
  .PD:
	mov rdi, rdx						; Destination = PDP + Offset
	mov rax, r10						; Calculate Offset into PD
	shr rax, 18
	and rax, 0x3FFFFFF8
	mov rdx, _PD						; PD Address
	add rdx, rax						;    += Offset
	bt QWORD [rdi], 0					; Does PD Exist?
	jc .PT								; Yes, goto .PT
	xor rax, rax
	mov rcx, 0x1000
	call _PMM_alloc						; Allocate a page for PD
	jc .Error
	call _VMM_RefInc					; Increase Reference
	or rax, (_GLOBAL | _WRITE | _PRESENT)
	stosq								; Map PD
	invlpg [rdx]						; Invalidate Recursive Address
  .PT:
	mov rdi, rdx						; Destination = PD + Offset
	mov rax, r10						; Calculate Offset into PT
	shr rax, 9
	mov rdx, 0x7FFFFFFFF8
	and rax, rdx
	mov rdx, _PT						; PT Address
	add rdx, rax						;    += Offset
	bt QWORD [rdi], 0					; Does PT Exist?
	jc .PE								; Yes, goto .PE
	xor rax, rax
	mov rcx, 0x1000
	call _PMM_alloc						; Allocate a page for PT
	jc .Error
	call _VMM_RefInc					; Increase Reference
	or rax, (_GLOBAL | _WRITE | _PRESENT)
	stosq								; Map PT
	invlpg [rdx]						; Invalidate Recursive Address
  .PE:
	mov rdi, rdx						; Destination = PE + Offset
	mov rax, r15						; Get Physical Address of entry
	bt rbx, 0							; Is this Writable?
	jnc .CheckUser						; No, Skip write flag
	or rax, _WRITE						; set write flag
  .CheckUser:
	bt rbx, 1							; Is this R3?
	jnc .CheckExec						; No, Skip User Flag
	or rax, _USER						; Set User Flag
  .CheckExec:
	bt rbx, 2							; Is this exec?
	jnc .MapPage						; No, Skip EXEC Flag
	bts rax, _EXEC						; Set EXEC Flag
  .MapPage:
	or rax, _PRESENT					; Set Present Flag
	stosq								; Map Entry
	invlpg [r10]						; Invalidate Virtual Address
	clc									; CF = 0 (no error)
  .Return:
	pop r10								; Restore Modified Registers
	pop r15
	pop rdi
	pop rdx
	pop rcx
	pop rax
	ret									; Return
  .Error:
	stc									; CF = 1
	jmp .Return							; Goto .Return
