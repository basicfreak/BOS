#ifndef				_PCI_H_INCLUDED
	#define				_PCI_H_INCLUDED
	
	#include <api.h>

	typedef struct PCI_HEADER_00 {
		uint16_t VendorID, DeviceID, Command, Status;
		uint8_t Rev, ProgIF, SubClass, Class, CachLineSize, LatencyTimer, HeaderType, BIST;
		uint32_t BAR0, BAR1, BAR2, BAR3, BAR4, BAR5, CISPointer;
		uint16_t SubSysVID, SubSysID;
		uint32_t ExpRomBase;
		uint16_t CapPointer, reserved0;
		uint32_t reserved1;
		uint8_t IntLine, IntPin, MinGrant, MaxLatency;
		uint32_t PAD[48];
	}__attribute__((packed)) PCI_H_00;

	typedef struct PCI_HEADER_01 {
		uint16_t VendorID, DeviceID, Command, Status;
		uint8_t Rev, ProgIF, SubClass, Class, CachLineSize, LatencyTimer, HeaderType, BIST;
		uint32_t BAR0, BAR1;
		uint8_t PriBusNum, SecBusNum, SubBusNum, SecLatency, IOBase, IOLimit;
		uint16_t SecStat, MemBase, MemLimit, PreFetchMemBase, PreFetchMemLimit;
		uint32_t PreFetchMemBase_High, PreFetchMemLimit_High;
		uint16_t IOBase_High, IOLimit_High;
		uint8_t CapPointer, reserved0[3];
		uint32_t ExpRomBase;
		uint8_t IntLine, IntPin;
		uint16_t BridgeCtrl;
		uint32_t PAD[48];
	}__attribute__((packed)) PCI_H_01;

	typedef struct PCI_HEADER_02 {
		uint16_t VendorID, DeviceID, Command, Status;
		uint8_t Rev, ProgIF, SubClass, Class, CachLineSize, LatencyTimer, HeaderType, BIST;
		uint32_t CardBusBase;
		uint8_t CapPointer, reserved0;
		uint16_t SecStat;
		uint8_t PCIBusNum, CardBusBusNum, SubordBusNum, CardBusLatencyTimer;
		uint32_t MemBase0, MemLimit0, MemBase1, MemLimit1, IOBase0, IOLimit0, IOBase1, IOLimit1;
		uint8_t IntLine, IntPin;
		uint16_t BridgeCtrl, SubSysID, SubSysVID;
		uint32_t LegacyBaseAddr, PAD[46];
	}__attribute__((packed)) PCI_H_02;

#endif