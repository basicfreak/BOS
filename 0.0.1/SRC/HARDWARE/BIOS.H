/*
./DRIVERSRC/HARDWARE/BIOS.H
*/

#ifndef				_BIOS_H_INCLUDED
	#define				_BIOS_H_INCLUDED
	///#define				DEBUG
	
	#include <FORMATTING.H>
	/**
		0x0400 (4 words);	IO ports for COM1-COM4 serial (each address is 1 word, zero if none);
		0x0408 (3 words);	IO ports for LPT1-LPT3 parallel (each address is 1 word, zero if none);
		0x040E (word);	 	EBDA base address >> 4 (usually!);
		0x0410 (word);	 	packed bit flags for detected hardware
		0x412	(5 Bytes)	UNKNOWN
		0x0417 (word);		keyboard state flags
		0x419	(5 Bytes)	UNKNOWN
		0x041E (32 bytes);	keyboard buffer
		0x43E	(11 Bytes)	UNKNOWN
		0x0449 (byte);		Display Mode
		0x044A (word);	 	Number of columns in text mode
		0x44C	(23 Bytes)	UNKNOWN
		0x0463 (2 bytes, 	taken as a word);	 base IO port for video
		0x445	(7 Bytes)	UNKNOWN
		0x046C (word);	 	# of IRQ0 timer ticks since boot
		0x46E	(7 Bytes)	UNKNOWN
		0x0475 (byte);	 	# of hard disk drives detected
		0x476	(10 Bytes)	UNKNOWN
		0x0480 (word);	 	keyboard buffer start
		0x0482 (word);	 	keyboard buffer end
		0x484	(19 Bytes)	UNKNOWN
		0x0497 (byte);	 	last keyboard LED/Shift key state
	**/
	
	#define BDA_BASE 0x400
	
	typedef struct BIOS_Data_Area {
		uint16_t	COM[4];
		uint16_t	LPT[3];
		uint16_t	EBDA_base;
		uint16_t	packet_bit_flags;
		uint8_t		UNKNOWN_1[5];
		uint16_t	KB_state_flags;
		uint8_t		UNKNOWN_2[5];
		uint8_t		KB_buffer[32];
		uint8_t		UNKNOWN_3[10];
		uint8_t		Video_mode;
		uint16_t	COLUMNS;
		uint8_t		UNKNOWN_4[23];
		uint8_t		Video_base[2];
		uint8_t		UNKNOWN_5[6];
		uint16_t	timer_ticks;
		uint8_t		UNKNOWN_6[7];
		uint8_t		HDD_count;
		uint8_t		UNKNOWN_7[9];
		uint16_t	KB_buffer_base;
		uint16_t	KB_buffer_end;
		uint8_t		UNKNOWN_8[19];
		uint8_t		KB_LED_state;
	} *BDA;
	
	void _BIOS_init(void);
	
	uint16_t _BIOS_COM(int off);
	uint16_t _BIOS_LPT(int off);
	uint16_t _BIOS_EBDA_base(void);
	uint16_t _BIOS_packet_bit_flags(void);
	uint16_t _BIOS_KB_state_flags(void);
	uint8_t _BIOS_KB_buffer(int off);
	uint8_t _BIOS_Video_mode(void);
	uint16_t _BIOS_COLUMNS(void);
	uint16_t _BIOS_Video_base(void);
	uint16_t _BIOS_timer_ticks(void);
	uint8_t _BIOS_HDD_count(void);
	uint16_t _BIOS_KB_buffer_base(void);
	uint16_t _BIOS_KB_buffer_end(void);
	uint8_t _BIOS_KB_LED_state(void);
	
#endif
