/*
./DRIVERSRC/HARDWARE/BIOS.C
*/

#include "BIOS.H"
#ifdef DEBUG
	#include <stdio.h>
#endif

BDA _BDA;

void _BIOS_init()
{
	_BDA = (BDA) 0x400;
	#ifdef DEBUG
		printf("\nBIOS Data Area (BDA)\n");
		printf("COM1 = 0x%x\tCOM2 = 0x%x\tCOM3 = 0x%x\tCOM4 = 0x%x\n", _BIOS_COM(1), _BIOS_COM(2), _BIOS_COM(3), _BIOS_COM(4));
		printf("LPT1 = 0x%x\tLPT2 = 0x%x\tLPT3 = 0x%x\n", _BIOS_LPT(1), _BIOS_LPT(2), _BIOS_LPT(3));
		printf("EBDA Base Address = 0x%x\n", _BIOS_EBDA_base());
		printf("Packet Bit Flags = 0x%x\n", _BIOS_packet_bit_flags());
		printf("Keyboard State Flags = 0x%x\n", _BIOS_KB_state_flags());
		printf("Video Mode = 0x%x\tCols = %i\t", _BIOS_Video_mode(), _BIOS_COLUMNS());
		printf("Base Address = 0x%x\n", _BIOS_Video_base());
		printf("Timer Ticks = %i\n",_BIOS_timer_ticks());
		printf("Hard Disk Drive Count = %i\n",_BIOS_HDD_count());
		printf("Keyboard Buffer Base = 0x%x\tEnd = 0x%x\tLEDs = 0x%x\n",_BIOS_KB_buffer_base(), _BIOS_KB_buffer_end(), _BIOS_KB_LED_state());
	#endif
}

uint16_t _BIOS_COM(int off)
{
	return _BDA->COM[off-1];
}

uint16_t _BIOS_LPT(int off)
{
	return _BDA->LPT[off-1];
}

uint16_t _BIOS_EBDA_base()
{
	return _BDA->EBDA_base;
}

uint16_t _BIOS_packet_bit_flags()
{
	return _BDA->packet_bit_flags;
}

uint16_t _BIOS_KB_state_flags()
{
	return _BDA->KB_state_flags;
}

uint8_t _BIOS_KB_buffer(int off)
{
	return _BDA->KB_buffer[off];
}

uint8_t _BIOS_Video_mode()
{
	return _BDA->Video_mode;
}

uint16_t _BIOS_COLUMNS()
{
	return _BDA->COLUMNS;
}

uint16_t _BIOS_Video_base()
{
	return (uint16_t) ((_BDA->Video_base[1] * 0x100) + _BDA->Video_base[0]);
}

uint16_t _BIOS_timer_ticks()
{
	return _BDA->timer_ticks;
}

uint8_t _BIOS_HDD_count()
{
	return _BDA->HDD_count;
}

uint16_t _BIOS_KB_buffer_base()
{
	return _BDA->KB_buffer_base;
}

uint16_t _BIOS_KB_buffer_end()
{
	return _BDA->KB_buffer_end;
}

uint8_t _BIOS_KB_LED_state()
{
	return _BDA->KB_LED_state;
}
