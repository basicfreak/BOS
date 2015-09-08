#ifndef				HAL_H_INCLUDED
	#define				HAL_H_INCLUDED

	#include <api.h>

	typedef struct HAL_ENT {
		void (*install)(uint32_t PrivateID);
		void (*read)(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
		void (*write)(uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
		void (*remove)(uint32_t PrivateID);
		uint32_t flags;
		uint32_t limit;
		char name[40];
	} __attribute__((packed)) HAL_t, *HAL_p;

	void _HAL_init(void);
	uint16_t mkHAL(HAL_p);
	void rmHAL(uint16_t);
	uint16_t getHAL(char* name);
	HAL_p getHALp(uint16_t);
	void installHAL(uint16_t HALid, uint32_t PrivateID);
	void readHAL(uint16_t HALid, uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
	void writeHAL(uint16_t HALid, uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
	void uninstallHAL(uint16_t HALid, uint32_t PrivateID);

	enum HAL_FLAGS {
		HAL_F_PRESENT	= 0x000001,
		HAL_F_READ		= 0x000002,
		HAL_F_WRITE		= 0x000004,
		HAL_F_DISK		= 0x000008,
		HAL_F_MEMORY	= 0x000010,
		HAL_F_HARDWARE	= 0x000020,
		HAL_F_MBR		= 0x100000,
		HAL_F_FLAT		= 0x200000
	};

#endif