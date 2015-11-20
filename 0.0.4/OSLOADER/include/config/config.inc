%ifndef __PXE_CONFIG_INC__INCLUDED__
%define __PXE_CONFIG_INC__INCLUDED__

[bits 16]

init_Config:
	mov si, BOOTCFG
	mov edx, DWORD 0x4000
	call ReadFile
	ret

processConfig:
	mov si, 0x4000
	.ConfigLoop:
		cmp BYTE [si], "K"
		je .LoadKernel
		cmp BYTE [si], "k"
		je .LoadBinaryKernel
		cmp BYTE [si], "M"
		je .LoadModule
		cmp BYTE [si], "B"
		je .BootKernel
		cmp BYTE [si], "V"
		je .Video
		inc si
		cmp si, 0x4200
		jge ERROR
		jmp .ConfigLoop
	ret
	.LoadKernel:
		inc si
		mov di, si
		call loadName ; changes di and si
		mov si, Name

		mov edx, DWORD 0x300000 ; Load at 3MB to make sure we do not overlap durring processing
		call ReadFile
		mov esi, DWORD 0x300000
		call ProcessElf

		mov eax, DWORD [ElfHead.entry]
		mov DWORD [BOOTINF.KernelStart], eax
		mov eax, DWORD [KernelInfo.End]
		mov DWORD [BOOTINF.KernelEnd], eax
		mov DWORD [BOOTINF.ModList], 0x7200
		
		mov si, di
		jmp .ConfigLoop
	.LoadBinaryKernel:
		inc si
		mov di, si
		call loadName
		mov si, Name
		mov edx, DWORD 0x100000
		mov DWORD [BOOTINF.KernelStart], edx
		mov DWORD [BOOTINF.KernelEnd], edx
		call ReadFile
		add DWORD [BOOTINF.KernelEnd], eax
		mov DWORD [BOOTINF.ModList], 0x7200
		mov si, di
		jmp .ConfigLoop
	.LoadModule:
		inc si
		mov di, si
		call loadName

		mov si, 0x7200
		mov eax, DWORD [si]
		test eax, eax
		jz .firstMod
		.findSpace:
			add si, 8
			mov eax, DWORD [si]
			test eax, eax
			jnz .findSpace
			mov eax, DWORD [si - 4]
			test ax, 0xFFF
			jz .ReadMod
			shr eax, 12
			inc eax
			shl eax, 12
			jmp .ReadMod
		.firstMod:
			mov eax, DWORD [BOOTINF.KernelEnd]
			test ax, 0xFFF
			jz .ReadMod
			shr eax, 12
			inc eax
			shl eax, 12
	.ReadMod:
		mov DWORD [si], eax
		add si, 4
		mov DWORD [si], eax
		push si
		mov edx, eax
		mov si, Name
		call ReadFile
		pop si
		add DWORD [si], eax
		add BYTE [BOOTINF.ModCount], 1

		mov si, di
		jmp .ConfigLoop
	.BootKernel:
		push si
		mov si, BootMsg
		call puts
		pop si

		mov eax, DWORD [BOOTINF.KernelStart]
		test eax, eax
		jz ERROR
; Wait for key press
xor ax, ax
int 0x16
		test WORD [VBEMODE], 0xFFFF
		jz .continueBoot
		call SetSVGAMode
		.continueBoot:
		cli
		mov ebx, cr0
		or bl, 1
		mov cr0, ebx
		mov bx, 10h
		mov ds, bx
		mov es, bx
		mov gs, bx
		mov fs, bx
		mov ss, bx
		jmp DWORD 18h:.pmodeboot
[bits 32]
		.pmodeboot:
			mov ebx, BOOTINF
			jmp DWORD [BOOTINF.KernelStart]
			hlt
[bits 16]
	.Video:
		inc si

		mov edx, DWORD [si]
		call Text2Hex
		add si, 4

		mov WORD [REQUESTEDSVGA.Width], dx
		mov edx, DWORD [si]
		call Text2Hex
		add si, 4
		mov WORD [REQUESTEDSVGA.Height], dx

		mov edx, DWORD [si]
		call Text2Hex
		add si, 4
		mov BYTE [REQUESTEDSVGA.Depth], dl
		test dh, dh
		jz .video.noLFB
		mov BYTE [REQUESTEDSVGA.LFB], 1
		.video.noLFB:

		push si
		mov si, VideoMSG
		call puts

		call FindSVGAMode

		xor eax, eax
		mov ax, VBEINFO
		mov DWORD [BOOTINF.CurVBEInfo], eax
		
		mov si, DONEMSG
		call puts
		pop si

		jmp .ConfigLoop
		

loadName:
	mov si, Name
	.loop:
		cmp BYTE [di], ';'
		je .done
		mov al, BYTE [di]
		mov BYTE [si], al
		inc si
		inc di
		jmp .loop
	.done:
		mov BYTE [si], 0
		inc si
		inc di
		ret

Name times 128 db 0

BootMsg db "Entering Kernel...", 10, 13, "Press A Key To Continue...", 0
VideoMSG db "Locating Requested SVGA Mode...", 0

%endif