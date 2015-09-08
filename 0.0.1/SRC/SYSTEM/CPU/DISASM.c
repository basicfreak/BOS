/*
./DRIVERSRC/SYSTEM/CPU/DISASM.C
*/
#include "DISASM.H"
#include <STRING.H>

const char *X86_ASM_OPCODE[] = {
//	0			1			2			3			4			5			6			7			8			9			A			B			C			D			E			F	
	"ADD",		"ADD",		"ADD",		"ADD",		"ADD",		"ADD",		"PUSH ES",	"POP ES",	"OR",		"OR",		"OR",		"OR",		"OR",		"OR",		"PUSH CS",	"POP CS",		// 0
	"ADC",		"ADC",		"ADC",		"ADC",		"ADC",		"ADC",		"PUSH SS",	"POP SS",	"SBB",		"SBB",		"SBB",		"SBB",		"SBB",		"SBB",		"PUSH DS",	"POP DS",		// 1
	"AND",		"AND",		"AND",		"AND",		"AND",		"AND",		"ES",		"DAA",		"SUB",		"SUB",		"SUB",		"SUB",		"SUB",		"SUB",		"CS",		"DAS",			// 2
	"XOR",		"XOR",		"XOR",		"XOR",		"XOR",		"XOR",		"SS",		"AAA",		"CMP",		"CMP",		"CMP",		"CMP",		"CMP",		"CMP",		"DS",		"AAS",			// 3
	"INC EAX",	"INC ECX",	"INC EDX",	"INC EBX",	"INC ESP",	"INC EBP",	"INC ESI",	"INC RDI",	"DEC EAX",	"DEC ECX",	"DEC EDX",	"DEC EBX",	"DEC ESP",	"DEC EBP",	"DEC ESI",	"DEC EDI",		// 4
	"PUSH EAX",	"PUSH ECX",	"PUSH EDX",	"PUSH EBX",	"PUSH ESP",	"PUSH EBP",	"PUSH ESI",	"PUSH EDI",	"POP EAX",	"POP ECX",	"POP EDX",	"POP EBX",	"POP ESP",	"POP EBP",	"POP ESI",	"POP EDI",		// 5
	"PUSHA",	"POPA",		"BOUND",	"ARPL",		"FS",		"GS",		"OPSIZE",	"ADSIZE",	"PUSH LV",	"IMUL",		"PUSH LB",	"IMUL",		"INSB",		"INSW",		"OUTSB",	"OUTSW",		// 6
	"JO",		"JNO",		"JB",		"JNB",		"JZ",		"JNZ",		"JBE",		"JA",		"JS",		"JNS",		"JP",		"JNP",		"JL",		"JNL",		"JLE",		"JNLE",			// 7
	"ADD",		"ADD",		"SUB",		"SUB",		"TEST",		"TEST",		"XCHG",		"XCHG",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"LEA",		"MOV",		"POP EV",		// 8
	"NOP",		"XCHG",		"XCHG",		"XCHG",		"XCHG",		"XCHG",		"XCHG",		"XCHG",		"CBW",		"CWD",		"CALL",		"WAIT",		"PUSHF",	"POPF",		"SAHF",		"LAHF",			// 9
	"MOV",		"MOV",		"MOV",		"MOV",		"MOVSB",	"MOVSW",	"CMPSB",	"CMPSW",	"TEST",		"TEST",		"STOSB",	"STOSW",	"LOADSB",	"LOADSW",	"SCASB",	"SCASW",		// A
	"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",		"MOV",			// B
	"#2",		"#2",		"RETN",		"RETN",		"LES",		"LDS",		"MOV",		"MOV",		"ENTER",	"LEAVE",	"RETF",		"RETF",		"INT3",		"INT",		"INTO",		"IRET",			// C
	"#2",		"#2",		"#2",		"#2",		"AAM",		"AAD",		"SALC",		"XLAT",		"ESC",		"ESC",		"ESC",		"ESC",		"ESC",		"ESC",		"ESC",		"ESC",			// D
	"LOOPNZ",	"LOOPZ",	"LOOP",		"JCXZ",		"IN",		"IN",		"OUT",		"OUT",		"CALL",		"JMP",		"JMP",		"JMP",		"IN",		"IN",		"OUT",		"OUT",			// E
	"LOCK",		"INT1",		"REPNE",	"REP",		"HLT",		"CMC",		"#3",		"#3",		"CLC",		"STC",		"CLI",		"STI",		"CLD",		"STD",		"$4",		"#5"			// F
};

char *DisASM(uint8_t Asm)
{
	return (char *)X86_ASM_OPCODE[Asm];
	/*switch (Asm) {
		case 0x37:
			return "AAA";
		case 0xD5:
			return "AAD";
		case 0xD4:
			return "AAM";
		case 0x3F:
			return "AAS";
		case 0x98:
			return "CBW";
		case 0xF8:
			return "CLC";
		case 0xFC:
			return "CLD";
		case 0xFA:
			return "CLI";
		case 0xF5:
			return "CMC";
		case 0xA6:
			return "CMPSB";
		case 0xA7:
			return "CMPSW";
		case 0x99:
			return "CWD";
		case 0x27:
			return "DAA";
		case 0x2F:
			return "DAS";
		case 0xF4:
			return "HLT";
		case 0xCE:
			return "INTO";
		case 0xE3:
			return "JCXZ";
		case 0xE9:
			return "JMP";
		case 0x9F:
			return "LAHF";
		case 0xC5:
			return "LDS";
		case 0x8D:
			return "LEA";
		case 0xC4:
			return "LES";
		case 0xF0:
			return "LOCK";
		case 0xAC:
			return "LODSB";
		case 0xAD:
			return "LODSW";
		case 0xA4:
			return "MOVSB";
		case 0xA5:
			return "MOVSW";
		case 0x90:
			return "NOP";
		case 0x9D:
			return "POPF";
		case 0x9C:
			return "PUSHF";
		case 0x9E:
			return "SAHF";
		case 0xAE:
			return "SCASB";
		case 0xAF:
			return "SCASW";
		case 0xF9:
			return "STC";
		case 0xFD:
			return "STD";
		case 0xFB:
			return "STI";
		case 0xAA:
			return "STOSB";
		case 0xAB:
			return "STOSW";
		case 0x9B:
			return "WAIT";
		case 0xD7:
			return "XLAT";
		case 0xA0:
			return "MOV-AL";
		case 0xA1:
			return "MOV-(E)AX";
		case 0xA2:
			return "MOV->AL";
		case 0xA3:
			return "MOV->(E)AX";
		case 0x88:
		case 0x89:
		case 0x8A:
		case 0x8B:
		case 0xBA:
		case 0x8C:
		case 0xB0:
		case 0xB8:
		case 0xC6:
		case 0xC7:
			return "MOV";
		case 0x1E:
			return "PUSH DS";
		case 0x1F:
			return "POP DS";
		case 0x06:
			return "PUSH ES";
		case 0x07:
			return "POP ES";
		case 0x16:
			return "PUSH SS";
		case 0x17:
			return "POP SS";
		case 0x0E:
			return "PUSH CS";
		case 0x50:
			return "PUSH EAX";
		case 0x51:
			return "PUSH ECX";
		case 0x52:
			return "PUSH EDX";
		case 0x53:
			return "PUSH EBX";
		case 0x54:
			return "PUSH ESP";
		case 0x55:
			return "PUSH EBP";
		case 0x56:
			return "PUSH ESI";
		case 0x57:
			return "PUSH EDI";
		case 0x58:
			return "POP EAX";
		case 0x59:
			return "POP ECX";
		case 0x5A:
			return "POP EDX";
		case 0x5B:
			return "POP EBX";
		case 0x5C:
			return "POP ESP";
		case 0x5D:
			return "POP EBP";
		case 0x5E:
			return "POP ESI";
		case 0x5F:
			return "POP EDI";
		case 0x60:
			return "PUSHA";
		case 0x61:
			return "POPA";
		case 0x70:
			return "JO";
		case 0x71:
			return "JNO";
		case 0x72:
			return "JB";
		case 0x73:
			return "JNB";
		case 0x74:
			return "JZ";
		case 0x75:
			return "JNZ";
		case 0x76:
			return "JBE";
		case 0x77:
			return "JA";
		case 0x78:
			return "JS";
		case 0x79:
			return "JNS";
		case 0x7A:
			return "JP";
		case 0x7B:
			return "JNP";
		case 0x7C:
			return "JL";
		case 0x7D:
			return "JNL";
		case 0x7E:
			return "JLE";
		case 0x7F:
			return "JNLE";
		case 0x80:
		case 0x81:
			return "ADD";
		case 0x82:
		case 0x83:
		case 0x28:
		case 0x29:
		case 0x2A:
		case 0x2B:
		case 0x2C:
		case 0x2D:
			return "SUB";
		case 0x84:
		case 0x85:
			return "TEST";
		case 0x86:
		case 0x87:
			return "XCHG";
	}
	
	char *ret = "        ";
	stringf(ret, "0x%x", Asm);
	return ret;*/
}