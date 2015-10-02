#include <pcilib.h>
#include <io.h>

#define pciToaddr(x, y, z, r) ((uint32_t)((uint32_t)(x << 16) | (uint32_t)(y << 11) | (uint32_t)(z << 8) | (uint32_t)( r & 0xFC) | ((uint32_t) 0x80000000)))

enum _PCI_CONFIGURATION_PORTS {
    _PCI_CONFIG_ADDRESS = 0xCF8,
    _PCI_CONFIG_DATA = 0xCFC
};

uint32_t PCI_findByClass(uint8_t Class, uint8_t SubClass)
{
// __asm__ __volatile__ ("xchg %bx, %bx");
    uint16_t TempWord = (uint16_t)((Class << 8) | SubClass);
    for (uint8_t bus = 0; bus < 0xFF; bus++)
        for (uint8_t slot = 0; slot < 0x20; slot++)
            for (uint8_t function = 0; function < 8; function ++)
                if(TempWord == PCI_CONFIG_getWord(bus, slot, function, 10))
                    return (uint32_t) pciToaddr(bus, slot, function, 0);
	return 0xFFFFFFFF;
}

uint32_t PCI_findByID(uint16_t Vendor, uint16_t Device)
{
// __asm__ __volatile__ ("xchg %bx, %bx");
    uint32_t TempDouble = (uint32_t)((Device << 16) | Vendor);
    for (uint8_t bus = 0; bus < 0xFF; bus++)
        for (uint8_t slot = 0; slot < 0x20; slot++)
            for (uint8_t function = 0; function < 8; function ++)
                if(TempDouble == PCI_CONFIG_getDouble(bus, slot, function, 0))
                    return (uint32_t) pciToaddr(bus, slot, function, 0);
	return 0xFFFFFFFF;
}

void PCI_getConfig(void* Buffer, uint32_t Addr)
{
// __asm__ __volatile__ ("xchg %bx, %bx");
	uint32_t* Buf = (uint32_t*) Buffer;
    for(uint32_t x = 0; x < 0x40; Addr += 4) {
        outl((uint16_t)_PCI_CONFIG_ADDRESS, Addr);
        inl((uint16_t)_PCI_CONFIG_DATA, Buf[x++]);
    }
}

uint32_t PCI_CalculateAddr(uint8_t bus, uint8_t slot, uint8_t function)
{
    return (uint32_t) pciToaddr(bus, slot, function, 0);
}

uint8_t PCI_CONFIG_getByte(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
    uint32_t ret;
    outl((uint16_t)_PCI_CONFIG_ADDRESS, ((uint32_t) pciToaddr(bus, slot, function, offset)));
    inl((uint16_t)_PCI_CONFIG_DATA, ret);
    return (uint8_t) ((ret >> ((offset & 3) * 8)) & 0xFF);
}

uint16_t PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
    uint32_t ret;
    outl((uint16_t)_PCI_CONFIG_ADDRESS, ((uint32_t) pciToaddr(bus, slot, function, offset)));
    inl((uint16_t)_PCI_CONFIG_DATA, ret);
    return (uint16_t) ((ret >> ((offset & 2) * 8)) & 0xFFFF);
}

uint32_t PCI_CONFIG_getDouble(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
    uint32_t ret;
    outl((uint16_t)_PCI_CONFIG_ADDRESS, ((uint32_t) pciToaddr(bus, slot, function, offset)));
    inl((uint16_t)_PCI_CONFIG_DATA, ret);
    return ret;
}
