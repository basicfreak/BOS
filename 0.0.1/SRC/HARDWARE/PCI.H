/*
./DRIVERSRC/HARDWARE/PCI.H
*/

#ifndef				_PCI_H_INCLUDED
	#define				_PCI_H_INCLUDED
	
	#include <FORMATTING.H>
	#define pciToaddr(x, y, z, r) ((x << 16) | (y << 11) | (z << 8) | ( r & 0xFC) | ((uint32_t) 0x80000000))
	typedef struct _PCI_DEVICE_HEADER {
		uint8_t		BUS;
		uint8_t		DEVICE;
		uint8_t		FUNCTION;
		char*		DeviceName, VenderName, TypeName;
		uint16_t	VendorID;
		uint16_t	DeviceID;
		uint16_t	Command;
		uint16_t	Status;
		uint8_t		RevisionID;
		uint8_t		ProgIF;
		uint8_t		Subclass;
		uint8_t		Class;
		uint8_t		CacheSize;
		uint8_t		LatencyTimer;
		uint8_t		HeaderType;
		uint8_t		BIST;				// Built In Self Test
		uint16_t	HeadID;				// Private ID.
	}PCIDEV;

	typedef struct _PCI_HEAD_00h {
		uint32_t	BAR[6], CIS, ExROM;
		uint16_t	SybSysID, SubSysVendorID;
		uint8_t		CapabilitiesPointer, IntPIN, IntLine, MaxLatency, MinGrant;
	}PCIHEAD00h;
	
	typedef struct _PCI_HEAD_01h
	{
		uint32_t	BAR[2], PrefetchBase, PrefetchLimit, ExROM;
		uint16_t	SecStat, MemLimit, MemBase, PrefetchMemLimit, PrefetchMemBase, IOlimitUper, IObaseUper, BridgeCtrl;
		uint8_t		SecLatencyTimer, SubBus, SecBus, PriBus, IOlimit, IObase, CapabilitiesPointer, IntPIN, IntLine;
	}PCIHEAD01h;

	/* I'LL Do this later as I have no need now...
	typedef struct _PCI_HEAD_02h
	{
		uint32_t	ExCaBase, 
		uint16_t	SecStat, 
		uint8_t		CapabilitiesOffest, 
	}PCIHEAD02h;*/


	typedef struct _PCI_DEVICE_HEADERS {
		PCIDEV	Device[2048]; // But really would someone have this many?!? If so I need to increase this...
	}PCIDEVLIST;
	typedef struct _PCI_DEVICE_HEADERS00h {
		PCIHEAD00h	Device[1024]; // But really would someone have this many?!? If so I need to increase this...
	}PCIDEVLIST00h;
	typedef struct _PCI_DEVICE_HEADERS01h {
		PCIHEAD01h	Device[1024]; // But really would someone have this many?!? If so I need to increase this...
	}PCIDEVLIST01h;

	uint16_t _PCI_DEVICES;
	uint16_t _PCI_DEVICES00h;
	uint16_t _PCI_DEVICES01h;
	
	void _PCI_init(void);
	
	
#endif