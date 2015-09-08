;----------------------------------------------------
;                   config.inc
;                   2015-08-17
;                 Brian T. Hoover
;   USE AT OWN RISK, I assume no responsibilities!
;----------------------------------------------------

%ifndef __CONFIG_INC__INCLUDED__
%define __CONFIG_INC__INCLUDED__

[bits 16]

ReadConfig:
	xor ax, ax
	mov si, LOADCFGMSG
	call puts

	mov si, BootDir
	call FindFile
	jc ERROR

	mov si, BootConf
	mov ax, WORD [CurrentCluster]
	call FindFile
	jc ERROR

	mov edx, DWORD 0xEE00
	call LoadFile

	mov si, DONEMSG
	call puts

	.ProcessConfig:
		mov si, 0x3000
		.loop:
			mov dl, BYTE [es:si]
			inc si
			cmp dl, 'V'
			je .video
			cmp dl, '3'
			je .S3
			cmp dl, 'R'
			je .RootDir
			cmp dl, 'D'
			je .FindDir
			cmp dl, 'K'
			je .ReadKernel
			cmp dl, 'M'
			je .ReadModule
			cmp dl, 'B'
			je .BootSystem
			cmp dl, 0
			je .done
			jmp .loop
	.done:
		ret

;----------------------------------------------------
;                  Video Settings
;----------------------------------------------------
	.video:
		mov edx, DWORD [es:si]
		call Text2Hex
		add si, 4
		mov WORD [VBEWidth], dx

		mov edx, DWORD [es:si]
		call Text2Hex
		add si, 4
		mov WORD [VBEHeight], dx

		mov edx, DWORD [es:si]
		call Text2Hex
		add si, 4
		mov BYTE [VBEDepth], dl
		test dh, dh
		jz .video.noLFB
		mov BYTE [VBELFB], 1
		.video.noLFB:
		call VBEFindMode

		cmp dx, 0xFFFF
		je ERROR

		call VBESetMode

		call GetPModeInterface
		
		jmp .loop

;----------------------------------------------------
;                    S3 Settings
;----------------------------------------------------
	.S3:
		call S3LFBEnable
		jmp .loop

;----------------------------------------------------
;                 BACK TO ROOT DIR
;----------------------------------------------------
	.RootDir:
		mov WORD [CurrentDirectoryCluster], 0
		jmp .loop

;----------------------------------------------------
;                   FIND DIRECTORY
;----------------------------------------------------
	.FindDir:
		mov ax, WORD [CurrentDirectoryCluster]
		call FindFile
		jc ERROR
		mov ax, WORD [CurrentCluster]
		mov WORD [CurrentDirectoryCluster], ax
		add si, 11
		jmp .loop

;----------------------------------------------------
;                    LOAD KERNEL
;----------------------------------------------------
	.ReadKernel:
		push si
			mov si, RKERNMSG
			call puts
		pop si
		
		mov ax, WORD [CurrentDirectoryCluster]
		call FindFile
		jc ERROR
		
		mov edx, DWORD 0x100000
		call LoadFileLong
		
		push si
			mov si, DONEMSG
			call puts
			mov si, PKERNMSG
			call puts
		pop si

		db 0x66, 0x67
			push esi
		db 0x66, 0x67
			push edi
		db 0x66, 0x67
			push ecx
		
		mov ESI, DWORD 0x100000

		call ProcessElf

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

		add si, 11

		push si
			mov si, DONEMSG
			call puts
		pop si
		jmp .loop

;----------------------------------------------------
;                    LOAD MODULE
;----------------------------------------------------
	.ReadModule:
		push si
			mov si, RMODMSG
			call puts
		pop si

		mov ax, WORD [CurrentDirectoryCluster]
		call FindFile
		jc ERROR

		mov di, 0x2E00
		mov al, BYTE[es:di + 27]
		test al, al
		jnz .NotFirst
		mov edx, DWORD [es:di + 19]
		db 0x66, 0x67
			inc edx
		mov di, 0x2600
		jmp .ReadTimeMod
		.NotFirst:
			xor bx, bx
			mov bl, al
			dec bx
			shl bx, 3
			mov di, 0x2600
			add di, bx
			mov edx, DWORD [es:di + 4]
			add di, 8
			db 0x66, 0x67
				inc edx
		.ReadTimeMod:
			mov DWORD [es:di], edx

		call LoadFileLong

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


		add si, 11

		push si
			mov si, DONEMSG
			call puts
		pop si
		jmp .loop


;----------------------------------------------------
;                   JUMP TO KERNEL
;----------------------------------------------------
	.BootSystem:
		push si
			mov si, BMSG
			call puts
		pop si
		xchg bx, bx
		mov eax, DWORD [KernelAddr]
		db 0x66, 0x67
			test eax, eax
		jz ERROR
		cli
		
		db 0x66, 0x67
		mov  ebx, cr0			; switch to pmode by
		or bl,1					; set pmode bit
		db 0x66, 0x67
		mov  cr0, ebx
		jmp 0x18:(.KJumpPM + 0xBE00)
[bits 32]
	.KJumpPM:
		mov bx, 0x10				; set pmode Data GDT ent
		mov ds, bx
		mov es, bx
		mov ss, bx
		mov ebx, DWORD 0xEC00
		jmp eax
		;jmp DWORD [KernelAddr]
		jmp 8:(.KJumpErr + 0xBE00)
	.KJumpErr:
		db 0x66, 0x67
		mov ebx, cr0
		db 0x66, 0x67
		and ebx, 0xFFFFFFFE
		db 0x66, 0x67
		mov cr0, ebx
		jmp 0:(.KJE + 0xBE00)

[bits 16]
	.KJE:
		mov bx, 0xBE0
		mov ds, bx
		;mov bx, 0xBE0
		mov es, bx
		mov bx, 0x500
		mov ss, bx
		jmp ERROR



CurrentDirectoryCluster dw 0

KernelAddr dd 0
KernelSize dd 0
KernelEnd dd 0

LOADCFGMSG db "Loading Boot Config...", 0
RKERNMSG db "Reading Kernel...", 0
PKERNMSG db "Processing Kernel...", 0
RMODMSG db "Reading Module...", 0
BMSG db "Jumping To Kernel...", 0

%endif ; __CONFIG_INC__INCLUDED__