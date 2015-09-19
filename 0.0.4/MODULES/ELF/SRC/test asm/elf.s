[bits 32]
[org 0]

start:
call entry

entry:
pop ebx
sub ebx, 5

mov al, 0x81
int 0xF1

testPrint:
mov esi, TestStr
add esi, ebx
push esi
mov edx, print
add edx, ebx
call edx
pop esi

mov al, 0x03
int 0xF1

mov edx, testPrint
add edx, ebx
jmp edx

putch:
	mov eax, [esp + 4]
	mov edx, 0xE9
	out dx, al
	ret

print:
	mov esi, [esp + 4]
	.PutsLoop:
		lodsb
		or al, al
		jz short PutsDone ; Check for Terminator (0)
		push eax
		mov edx, putch
		add edx, ebx
		call edx
		pop eax
		jmp short .PutsLoop
	PutsDone:
		ret

TestStr db "Hello World of PIC!", 0x0A, 0x0D, 0