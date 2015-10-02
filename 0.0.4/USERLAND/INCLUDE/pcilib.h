#ifndef _PCILIB_H_INCLUDE
	#define _PCILIB_H_INCLUDE

	#include <typedefines.h>

	uint32_t PCI_findByID(uint16_t, uint16_t);
	uint32_t PCI_findByClass(uint8_t, uint8_t);
	void PCI_getConfig(void*, uint32_t);
	uint8_t PCI_CONFIG_getByte(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	uint16_t PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	uint32_t PCI_CONFIG_getDouble(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);

#endif