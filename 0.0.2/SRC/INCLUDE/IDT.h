/*
./DRIVERSRC/SYSTEM/CPU/IDT.H
*/

#ifndef				IDT_H_INCLUDED
	#define				IDT_H_INCLUDED
	
	#include <KLIB.h>
	
	typedef struct idt_entry	// 64 Bytes
	{
		uint16_t base_lo;
		uint16_t sel;
		uint8_t always0;
		uint8_t flags;
		uint16_t base_hi;
	} __attribute__((packed)) IDTEntry_t, *IDTEntry_p;

	typedef struct idt_ptr		// 48 Bytes
	{
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed)) IDTPtr_t, *IDTPtr_p;

	// We have 256 IDT entries
	typedef struct idt_table	// 16,384 Bytes (16KB [4 Pages])
	{
		IDTEntry_t	Entry[256];
	} IDTTable_t, *IDTTable_p;


	typedef struct registers	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;    
	} regs;

	extern void LoadIDT(void);
	extern void IDT0(void);
	extern void IDT1(void);
	extern void IDT2(void);
	extern void IDT3(void);
	extern void IDT4(void);
	extern void IDT5(void);
	extern void IDT6(void);
	extern void IDT7(void);
	extern void IDT8(void);
	extern void IDT9(void);
	extern void IDT10(void);
	extern void IDT11(void);
	extern void IDT12(void);
	extern void IDT13(void);
	extern void IDT14(void);
	extern void IDT15(void);
	extern void IDT16(void);
	extern void IDT17(void);
	extern void IDT18(void);
	extern void IDT19(void);
	extern void IDT20(void);
	extern void IDT21(void);
	extern void IDT22(void);
	extern void IDT23(void);
	extern void IDT24(void);
	extern void IDT25(void);
	extern void IDT26(void);
	extern void IDT27(void);
	extern void IDT28(void);
	extern void IDT29(void);
	extern void IDT30(void);
	extern void IDT31(void);
	extern void IDT32(void);
	extern void IDT33(void);
	extern void IDT34(void);
	extern void IDT35(void);
	extern void IDT36(void);
	extern void IDT37(void);
	extern void IDT38(void);
	extern void IDT39(void);
	extern void IDT40(void);
	extern void IDT41(void);
	extern void IDT42(void);
	extern void IDT43(void);
	extern void IDT44(void);
	extern void IDT45(void);
	extern void IDT46(void);
	extern void IDT47(void);
	extern void IDT48(void);
	extern void IDT49(void);
	extern void IDT50(void);
	extern void IDT51(void);
	extern void IDT52(void);
	extern void IDT53(void);
	extern void IDT54(void);
	extern void IDT55(void);
	extern void IDT56(void);
	extern void IDT57(void);
	extern void IDT58(void);
	extern void IDT59(void);
	extern void IDT60(void);
	extern void IDT61(void);
	extern void IDT62(void);
	extern void IDT63(void);
	extern void IDT64(void);
	extern void IDT65(void);
	extern void IDT66(void);
	extern void IDT67(void);
	extern void IDT68(void);
	extern void IDT69(void);
	extern void IDT70(void);
	extern void IDT71(void);
	extern void IDT72(void);
	extern void IDT73(void);
	extern void IDT74(void);
	extern void IDT75(void);
	extern void IDT76(void);
	extern void IDT77(void);
	extern void IDT78(void);
	extern void IDT79(void);
	extern void IDT80(void);
	extern void IDT81(void);
	extern void IDT82(void);
	extern void IDT83(void);
	extern void IDT84(void);
	extern void IDT85(void);
	extern void IDT86(void);
	extern void IDT87(void);
	extern void IDT88(void);
	extern void IDT89(void);
	extern void IDT90(void);
	extern void IDT91(void);
	extern void IDT92(void);
	extern void IDT93(void);
	extern void IDT94(void);
	extern void IDT95(void);
	extern void IDT96(void);
	extern void IDT97(void);
	extern void IDT98(void);
	extern void IDT99(void);
	extern void IDT100(void);
	extern void IDT101(void);
	extern void IDT102(void);
	extern void IDT103(void);
	extern void IDT104(void);
	extern void IDT105(void);
	extern void IDT106(void);
	extern void IDT107(void);
	extern void IDT108(void);
	extern void IDT109(void);
	extern void IDT110(void);
	extern void IDT111(void);
	extern void IDT112(void);
	extern void IDT113(void);
	extern void IDT114(void);
	extern void IDT115(void);
	extern void IDT116(void);
	extern void IDT117(void);
	extern void IDT118(void);
	extern void IDT119(void);
	extern void IDT120(void);
	extern void IDT121(void);
	extern void IDT122(void);
	extern void IDT123(void);
	extern void IDT124(void);
	extern void IDT125(void);
	extern void IDT126(void);
	extern void IDT127(void);
	extern void IDT128(void);
	extern void IDT129(void);
	extern void IDT130(void);
	extern void IDT131(void);
	extern void IDT132(void);
	extern void IDT133(void);
	extern void IDT134(void);
	extern void IDT135(void);
	extern void IDT136(void);
	extern void IDT137(void);
	extern void IDT138(void);
	extern void IDT139(void);
	extern void IDT140(void);
	extern void IDT141(void);
	extern void IDT142(void);
	extern void IDT143(void);
	extern void IDT144(void);
	extern void IDT145(void);
	extern void IDT146(void);
	extern void IDT147(void);
	extern void IDT148(void);
	extern void IDT149(void);
	extern void IDT150(void);
	extern void IDT151(void);
	extern void IDT152(void);
	extern void IDT153(void);
	extern void IDT154(void);
	extern void IDT155(void);
	extern void IDT156(void);
	extern void IDT157(void);
	extern void IDT158(void);
	extern void IDT159(void);
	extern void IDT160(void);
	extern void IDT161(void);
	extern void IDT162(void);
	extern void IDT163(void);
	extern void IDT164(void);
	extern void IDT165(void);
	extern void IDT166(void);
	extern void IDT167(void);
	extern void IDT168(void);
	extern void IDT169(void);
	extern void IDT170(void);
	extern void IDT171(void);
	extern void IDT172(void);
	extern void IDT173(void);
	extern void IDT174(void);
	extern void IDT175(void);
	extern void IDT176(void);
	extern void IDT177(void);
	extern void IDT178(void);
	extern void IDT179(void);
	extern void IDT180(void);
	extern void IDT181(void);
	extern void IDT182(void);
	extern void IDT183(void);
	extern void IDT184(void);
	extern void IDT185(void);
	extern void IDT186(void);
	extern void IDT187(void);
	extern void IDT188(void);
	extern void IDT189(void);
	extern void IDT190(void);
	extern void IDT191(void);
	extern void IDT192(void);
	extern void IDT193(void);
	extern void IDT194(void);
	extern void IDT195(void);
	extern void IDT196(void);
	extern void IDT197(void);
	extern void IDT198(void);
	extern void IDT199(void);
	extern void IDT200(void);
	extern void IDT201(void);
	extern void IDT202(void);
	extern void IDT203(void);
	extern void IDT204(void);
	extern void IDT205(void);
	extern void IDT206(void);
	extern void IDT207(void);
	extern void IDT208(void);
	extern void IDT209(void);
	extern void IDT210(void);
	extern void IDT211(void);
	extern void IDT212(void);
	extern void IDT213(void);
	extern void IDT214(void);
	extern void IDT215(void);
	extern void IDT216(void);
	extern void IDT217(void);
	extern void IDT218(void);
	extern void IDT219(void);
	extern void IDT220(void);
	extern void IDT221(void);
	extern void IDT222(void);
	extern void IDT223(void);
	extern void IDT224(void);
	extern void IDT225(void);
	extern void IDT226(void);
	extern void IDT227(void);
	extern void IDT228(void);
	extern void IDT229(void);
	extern void IDT230(void);
	extern void IDT231(void);
	extern void IDT232(void);
	extern void IDT233(void);
	extern void IDT234(void);
	extern void IDT235(void);
	extern void IDT236(void);
	extern void IDT237(void);
	extern void IDT238(void);
	extern void IDT239(void);
	extern void IDT240(void);
	extern void IDT241(void);
	extern void IDT242(void);
	extern void IDT243(void);
	extern void IDT244(void);
	extern void IDT245(void);
	extern void IDT246(void);
	extern void IDT247(void);
	extern void IDT248(void);
	extern void IDT249(void);
	extern void IDT250(void);
	extern void IDT251(void);
	extern void IDT252(void);
	extern void IDT253(void);
	extern void IDT254(void);
	extern void IDT255(void);
	
	void _IDT_init(void);

	void IDT_HANDLER(regs *r);
	void newHandler(void (*IR)(regs *r), uint8_t idtno);
	
#endif