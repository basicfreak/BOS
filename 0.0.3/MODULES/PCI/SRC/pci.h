#ifndef				_PCI_H_INCLUDED
	#define				_PCI_H_INCLUDED
	
	#include <api.h>

	#define MAX_PCI 1024

	#define pciToaddr(x, y, z, r) ((x << 16) | (y << 11) | (z << 8) | ( r & 0xFC) | ((uint32_t) 0x80000000))

	typedef struct _PCI_DEVICES { // 16 Bytes
		uint8_t BUS, SLOT, FUNCTION;
		uint16_t Vendor, Device;
		uint8_t Class, SubClass, ProgIF, HeaderType;
		uint8_t INTLine;
		uint32_t BAR0;
	}__attribute__((packed)) _PCI_DEV_t, *_PCI_DEV_p;

	typedef struct _PCI_DEVICES_TABLE {
		_PCI_DEV_p Device[MAX_PCI];
	}__attribute__((packed)) _PCI_TBL_t, *_PCI_TBL_p;

	enum _PCI_CONFIGURATION_PORTS {
		_PCI_CONFIG_ADDRESS = 0xCF8,
		_PCI_CONFIG_DATA = 0xCFC
	};

#endif