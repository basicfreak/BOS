#ifndef				_PCIGET_H_INCLUDED
	#define				_PCIGET_H_INCLUDED

	#include <api.h>

	uint8_t _PCI_CONFIG_getByte(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	uint16_t _PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	uint32_t _PCI_CONFIG_getLong(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	void _PCI_CONFIG_Dump(uint8_t bus, uint8_t slot, uint8_t function, void* Buffer); // Buffer is written 256 Bytes

#endif