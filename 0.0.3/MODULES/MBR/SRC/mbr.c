#include <hal.h>
#include <vfs.h>

void install(uint32_t PrivateID);
void remove(uint32_t PrivateID);

HAL_p MyHAL;

typedef struct PARTENTRY {
	uint8_t PAD[4];
	uint8_t FSid;
	uint8_t PAd[3];
	uint32_t StartLBA;
	uint32_t SizeLBA;
}__attribute__((packed)) PartEnt;

int init()
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	MyHAL = (HAL_p) calloc(sizeof(HAL_t));

	MyHAL->install = install;
	MyHAL->read = 0;
	MyHAL->write = 0;
	MyHAL->remove = remove;
	MyHAL->flags = (HAL_F_PRESENT | HAL_F_MBR);
	strcpy(MyHAL->name, "MBR");

	mkHAL(MyHAL);
	return 0;
}

void install(uint32_t PrivateID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	uint32_t TruePrID = (PrivateID >> 16);
	//PrivateID = HAL ID of Disk
	HAL_p DiskHAL = getHALp((uint16_t)(PrivateID & 0xFFFF));
	DEBUG_printf("MBR DISK HAL = 0x%x (%i)\n", (uint16_t)(PrivateID & 0xFFFF), (uint16_t)(PrivateID & 0xFFFF));
	if(DiskHAL && (DiskHAL->flags & (HAL_F_PRESENT | HAL_F_DISK))) { // Is it real and does it exist?
		if(DiskHAL->flags & HAL_F_MBR) {
			// This Is MBR
			void* MBRBuffer = calloc(512);
			//(uint16_t HALid, uint32_t PrivateID, void* Address, void* Buffer, uint32_t Length);
			readHAL((uint16_t)(PrivateID & 0xFFFF), TruePrID, 0, MBRBuffer, 1);

			PartEnt *PartitionList = (PartEnt*) ((uint32_t)MBRBuffer + 0x1BE);

			for(int x = 0; x < 4; x++) {
				DEBUG_printf("PARTITION %i\tStart 0x%x\tLength 0x%x\tFSid 0x%x\n", x, PartitionList[x].StartLBA, PartitionList[x].SizeLBA, PartitionList[x].FSid);
				if(PartitionList[x].FSid == 0x05) { // Extended Partition Table
					void *ExPart = calloc(512);
					readHAL((uint16_t)(PrivateID & 0xFFFF), TruePrID, (void*)PartitionList[x].StartLBA, ExPart, 1);

					PartEnt *ExPartList = (PartEnt*) ((uint32_t)ExPart + 0x1BE);

					for(int y = 0; y < 4; y++) {
						DEBUG_printf("\tExtended Partition %i\tStart 0x%x\tLength 0x%x\tFSid 0x%x\n", y, ExPartList[y].StartLBA, ExPartList[y].SizeLBA, ExPartList[y].FSid);
						// Mount Ext Partitions
						if (ExPartList[y].FSid) {
							PART_p MyPart = (PART_p) calloc((sizeof(PART_t)));
							MyPart->HALid = (uint16_t)(PrivateID & 0xFFFF);
							MyPart->FSid = ExPartList[y].FSid; // FAT12
							MyPart->Start = (PartitionList[x].StartLBA + ExPartList[y].StartLBA);
							MyPart->Length = ExPartList[y].SizeLBA; // 1.44MB
							MyPart->PrivateID = TruePrID;
							uint32_t MyPartID = mkPartition(MyPart);
							fmount(MyPartID);
						}
					}

					free(ExPart, 512);
				} else if (PartitionList[x].FSid){
					// Mount Main Partition
					PART_p MyPart = (PART_p) calloc((sizeof(PART_t)));
					MyPart->HALid = (uint16_t)(PrivateID & 0xFFFF);
					MyPart->FSid = PartitionList[x].FSid; // FAT12
					MyPart->Start = PartitionList[x].StartLBA;
					MyPart->Length = PartitionList[x].SizeLBA; // 1.44MB
					MyPart->PrivateID = TruePrID;
					uint32_t MyPartID = mkPartition(MyPart);
					fmount(MyPartID);
				}
			}

uint8_t* MBRBuf = (uint8_t*) MBRBuffer;
for(int x=0;x<512;x++)
	DEBUG_printf("%x ", MBRBuf[x]);
			free(MBRBuffer, 512);
		}
	}
}

void remove(uint32_t PrivateID)
{
#ifdef DEBUG
	DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
	//PrivateID = HAL ID of Disk
}
