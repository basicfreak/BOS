
#include <pci.h>

/**
LOCAL VARIABLES
**/

_PCI_TBL_p PCI_Table = (_PCI_TBL_p) 0;
uint8_t PCI_Count = 1;

enum _PCI_CONFIGURATION_PORTS {
    _PCI_CONFIG_ADDRESS = 0xCF8,
    _PCI_CONFIG_DATA = 0xCFC
};

uint8_t _PCI_CONFIG_getByte(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint16_t _PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint32_t _PCI_CONFIG_getLong(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
void _PCI_scanDevices(void);


/*
CONFIG
BITS:
31          30-24       23-16       15-11       10-8        7-2         1-0
ENABLE      RESERVED    BUS NUMBER  DEVICE NUM  FUNCTION #  REGISTER #  00
*/

/**
LOCAL FUNTIONS
**/ 
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

void _PCI_scanDevices()
{
    uint16_t list = 0;
    for (uint8_t bus = 0; bus < 0xFF; bus++)
        for (uint8_t slot = 0; slot < 0x20; slot++) {
            for (uint8_t function = 0; function < 8; function ++) {
                if ((_PCI_CONFIG_getWord(bus, slot, function, 0) != 0xFFFF) && (_PCI_CONFIG_getWord(bus, slot, function, 0) != 0)) {        // Does device exist?
                    PCI_Table->Device[PCI_Count].BUS = bus;
                    PCI_Table->Device[PCI_Count].SLOT = slot;
                    PCI_Table->Device[PCI_Count].FUNCTION = function;
                    PCI_Table->Device[PCI_Count].Vendor = _PCI_CONFIG_getWord(bus, slot, function, 0);
                    PCI_Table->Device[PCI_Count].Device = _PCI_CONFIG_getWord(bus, slot, function, 2);
                    PCI_Table->Device[PCI_Count].Class = _PCI_CONFIG_getByte(bus, slot, function, 11);
                    PCI_Table->Device[PCI_Count].SubClass = _PCI_CONFIG_getByte(bus, slot, function, 10);
                    PCI_Table->Device[PCI_Count].HeaderType = _PCI_CONFIG_getByte(bus, slot, function, 14);
                    PCI_Table->Device[PCI_Count].BaseAddr = _PCI_CONFIG_getLong(bus, slot, function, 16);
                    PCI_Table->Device[PCI_Count].INTline = _PCI_CONFIG_getByte(bus, slot, function, 0x3C);
                    PCI_Table->Device[PCI_Count].HALid = 0;
                    PCI_Count++;
                }
            }
        }
}

/**
PUBLIC FUNCTIONS
**/

int init()
{
    PCI_Table = calloc(4096);
    PCI_Count = 1;
    _PCI_scanDevices();
    _API_Add_Symbol("PCI_GetDevice", (uint32_t)&PCI_GetDevice);
    _API_Add_Symbol("PCI_SetHAL", (uint32_t)&PCI_SetHAL);
    _API_Add_Symbol("PCI_GetBaseAddr", (uint32_t)&PCI_GetBaseAddr);
    _API_Add_Symbol("PCI_GetINTLine", (uint32_t)&PCI_GetINTLine);

    DEBUG_printf("Bus:Slot:Function\tVendor\tDevice\tClass\tSubClass\tHeadType\tBaseAddr\tINTLine\n");
    for(uint8_t x = 1; x < PCI_Count; x++) {
        DEBUG_printf("PCI_%x_%x_%x\t\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\n", PCI_Table->Device[x].BUS, PCI_Table->Device[x].SLOT,
            PCI_Table->Device[x].FUNCTION, PCI_Table->Device[x].Vendor, PCI_Table->Device[x].Device, PCI_Table->Device[x].Class,
            PCI_Table->Device[x].SubClass, PCI_Table->Device[x].HeaderType, PCI_Table->Device[x].BaseAddr, PCI_Table->Device[x].INTline);
    }
    return 0;
}

uint8_t PCI_GetDevice(uint16_t Vendor, uint16_t Device)
{
    for(uint8_t P = 1; P < PCI_Count; P++)
        if( (PCI_Table->Device[P].Vendor == Vendor) && (PCI_Table->Device[P].Device == Device) )
            return P;
    return 0;
}

void PCI_SetHAL(uint8_t PCI, uint8_t HAL)
{
    PCI_Table->Device[PCI].HALid = HAL;
}

uint32_t PCI_GetBaseAddr(uint8_t PCI)
{
    uint32_t BA = PCI_Table->Device[PCI].BaseAddr;
    uint32_t RET = 0;
    if(BA & 1) {
        //PORT
        RET = BA & 0xFFFFFFFc;
    } else {
        //MEMORY
        RET = BA & 0xFFFFFFF0;
    }
    return RET;
}

uint8_t PCI_GetINTLine(uint8_t PCI)
{
    return PCI_Table->Device[PCI].INTline;
}
