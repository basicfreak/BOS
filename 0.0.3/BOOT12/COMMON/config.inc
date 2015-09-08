;----------------------------------------------------
;                   config.inc
;                   2015-07-12
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

[bits 16]

ReadConfig:
;----------------------------------------------------
;            Read Boot Config Into Memory
;----------------------------------------------------
	call ReadRootDir	; Read Root Dir into memory

	mov si, BootDir		; Boot Directory Name
	call FindFileInDir	; Find Directory in Root Dir
	cmp ax, 1			; See if we have found Dir
	jne ERROR_STUB		; If Not ERROR

	db 0x66, 0x67
	mov eax, 0x11200
	call ReadFile		; Read Directory into memory

	mov si, BootConf	; Boot Config File Name
	call FindFileInDir	; Find Config in Dir
	cmp ax, 1			; Do we have
	jne ERROR_STUB		; No? ERROR

	db 0x66, 0x67
	mov eax, 0x7C00		; Tell ReadFile to move Config to 0x7C00
	call ReadFile		; Read Config into memory

;----------------------------------------------------
;                Process Boot Config
;----------------------------------------------------
	mov si, 0x7C00
	.ProcessConfig:
		.loop:
			mov dl, BYTE [si]
			inc si
			cmp dl, 'V'
			je .video
			cmp dl, '3'
			je .S3
			cmp dl, 'R'
			je .ReadRoot
			cmp dl, 'D'
			je .ReadDir
			cmp dl, 'K'
			je .ReadKernel
			cmp dl, 'M'
			je .ReadModule
			cmp dl, 'B'
			je .BootSystem
			cmp dl, 0
			je .done
		.loop_cont:
			jmp .loop

;----------------------------------------------------
;                  Video Settings
;----------------------------------------------------
	.video:
		mov edx, DWORD [si]
		call Text2Hex
		add si, 4
		mov WORD [VBEWidth], dx

		mov edx, DWORD [si]
		call Text2Hex
		add si, 4
		mov WORD [VBEHeight], dx

		mov edx, DWORD [si]
		call Text2Hex
		add si, 4
		mov BYTE [VBEDepth], dl
		test dh, dh
		jz .video.noLFB
		mov BYTE [VBELFB], 1
		.video.noLFB:
		call VBEFindMode

		cmp dx, 0xFFFF
		je ERROR_STUB

		call VBESetMode
		jmp .loop_cont

;----------------------------------------------------
;                    S3 Settings
;----------------------------------------------------
	.S3:
		call S3LFBEnable
		jmp .loop_cont


;----------------------------------------------------
;                   Read Root Dir
;----------------------------------------------------
.RReadMsg db "Reading Root Dir...", 0
	.ReadRoot:
push si
mov si, .RReadMsg
call puts
pop si
		call ReadRootDir
push si
mov si, CONFDoneMsg
call puts
pop si
		jmp .loop_cont

;----------------------------------------------------
;                     Read DIR
;----------------------------------------------------
.DFindMsg db "Finding Directory...", 0
.DLoadMsg db "Loading Directory...", 0
	.ReadDir:
push si
mov si, .DFindMsg
call puts
pop si
		push ax
		call FindFileInDir	; Find Directory in Root Dir
		cmp ax, 1			; See if we have found Dir
		jne ERROR_STUB		; If Not ERROR
push si
mov si, CONFDoneMsg
call puts
pop si
		add si, 11
		db 0x66, 0x67
		mov eax, 0x11200
push si
mov si, .DLoadMsg
call puts
pop si
		call ReadFile		; Read Directory into memory
push si
mov si, CONFDoneMsg
call puts
pop si
		pop ax
		jmp .loop_cont

;----------------------------------------------------
;                    Read Kernel
;----------------------------------------------------
.KReadingMsg db "Reading Kernel...", 0
.KFindingMsg db "Finding Kernel...", 0
.KProcessMsg db "Processing Kernel...", 0
	.ReadKernel:
push si
mov si, .KFindingMsg
call puts
pop si
		push ax
		call FindFileInDir	; Find Directory in Root Dir
		cmp ax, 1			; See if we have found Dir
		jne ERROR_STUB		; If Not ERROR
push si
mov si, CONFDoneMsg
call puts
pop si
		add si, 11
		db 0x66, 0x67
		mov eax, 0x100000
push si
mov si, .KReadingMsg
call puts
pop si
		call ReadFile		; Read Directory into memory
push si
mov si, CONFDoneMsg
call puts
pop si
		db 0x66, 0x67
		push esi
		db 0x66, 0x67
		push edi
		db 0x66, 0x67
		push ecx
		

		db 0x66, 0x67
		mov ESI, 0x100000
push si
mov si, .KProcessMsg
call puts
pop si
		call ProcessElf
push si
mov si, CONFDoneMsg
call puts
pop si
		mov di, 0x2E00 ; [es:di] = 0xEC00
		mov eax, DWORD [KernelAddr]
		mov DWORD [es:di + 15], eax
		mov eax, DWORD [KernelEnd]
		mov DWORD [es:di + 19], eax

		db 0x66, 0x67
		pop ecx
		db 0x66, 0x67
		pop edi
		db 0x66, 0x67
		pop esi
		pop ax
		jmp .loop_cont


;----------------------------------------------------
;                    Load Module
;----------------------------------------------------
.MFindMsg db "Finding Module...", 0
.MCalcMsg db "Calculating Module Location...", 0
.MLoadMsg db "Loading Module...", 0
	.ReadModule:
push si
mov si, .MFindMsg
call puts
pop si
;CurFileSectorCount
		pusha
		call FindFileInDir
		cmp ax, 1
		jne ERROR_STUB
push si
mov si, CONFDoneMsg
call puts
mov si, .MCalcMsg
call puts
pop si
		mov di, 0x2E00
		mov al, BYTE[es:di + 27]
		test al, al
		jnz .NotFirst
		mov eax, DWORD [es:di + 19]
		db 0x66, 0x67
		inc eax
		mov di, 0x2600
		jmp .ReadTimeMod
		.NotFirst:
			xor bx, bx
			mov bl, al
			dec bx
			shl bx, 3
			mov di, 0x2600
			add di, bx
			mov eax, DWORD [es:di + 4]
			add di, 8
			db 0x66, 0x67
			inc eax
		.ReadTimeMod:
			mov DWORD [es:di], eax
push si
mov si, CONFDoneMsg
call puts
mov si, .MLoadMsg
call puts
pop si
			call ReadFile
push si
mov si, CONFDoneMsg
call puts
pop si
			db 0x66, 0x67
			xor eax, eax
			mov ax, WORD [CurFileSectorCount]
			db 0x66, 0x67
			shl eax, 9
			add eax, DWORD [es:di]
			db 0x66, 0x67
			dec eax
			mov DWORD [es:di + 4], eax
			mov di, 0x2E00
			inc BYTE [es:di + 27]

		popa
		add si, 11
		jmp .loop_cont

;----------------------------------------------------
;                   Jump To Kernel
;----------------------------------------------------
	.BootSystem:
xchg bx, bx
		mov eax, DWORD [KernelAddr]
		db 0x66, 0x67
			test eax, eax
		jz ERROR_STUB
		cli
		
		db 0x66, 0x67
		mov  ebx, cr0			; switch to pmode by
		or bl,1					; set pmode bit
		db 0x66, 0x67
		mov  cr0, ebx
		jmp 0x18:.KJumpPM
[bits 32]
	.KJumpPM:
		mov bx, 0x10				; set pmode Data GDT ent
		mov ds, bx
		mov es, bx
		mov ss, bx
		mov ebx, DWORD 0xEC00
		jmp DWORD [KernelAddr]
		jmp 8:.KJumpErr
	.KJumpErr:
		db 0x66, 0x67
		mov ebx, cr0
		db 0x66, 0x67
		and ebx, 0xFFFFFFFE
		db 0x66, 0x67
		mov cr0, ebx
		jmp 0:.KJE

[bits 16]
	.KJE:
		mov bx, 0
		mov ds, bx
		mov bx, 0xBE0
		mov es, bx
		mov bx, 0x500
		mov ss, bx
		jmp ERROR_STUB
;----------------------------------------------------
;                 Done With Config
;----------------------------------------------------
	.done:
		ret

KernelAddr dd 0
KernelSize dd 0
KernelEnd dd 0

CONFDoneMsg db ".DONE!", 0x0A, 0x0D, 0
