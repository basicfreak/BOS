#include "pci.h"
#include "pciget.h"

uint8_t _PCI_CONFIG_getByte(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
    uint32_t add;
    add = (uint32_t) pciToaddr(bus, slot, function, offset);
    outl(_PCI_CONFIG_ADDRESS, add);
    return (uint8_t) ((inl(_PCI_CONFIG_DATA) >> ((offset & 3) * 8)) & 0xFF);
}

uint16_t _PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
    uint32_t add;
    add = (uint32_t) pciToaddr(bus, slot, function, offset);
    outl(_PCI_CONFIG_ADDRESS, add);
    return (uint16_t) ((inl(_PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
}

uint32_t _PCI_CONFIG_getLong(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
    uint32_t add;
    add = (uint32_t) pciToaddr(bus, slot, function, offset); //((Lbus << 16) | (Lslot << 11) | (Lfunction << 8) | ( Loffset & 0xFC) | ((uint32_t) 0x80000000));
    outl(_PCI_CONFIG_ADDRESS, add);
    return inl(_PCI_CONFIG_DATA);
}

void _PCI_CONFIG_Dump(uint8_t bus, uint8_t slot, uint8_t function, void* Buffer)
{
	uint32_t *Buff = (uint32_t *) Buffer;
	for(uint32_t x = 0; x < 0x40; x++)
		Buff[x] = _PCI_CONFIG_getLong(bus, slot, function, ((uint8_t) (x * 4)));
}