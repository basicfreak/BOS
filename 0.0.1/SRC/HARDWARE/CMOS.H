/*
./INCLUDE/CMOS.H
*/

#ifndef				CMOS_H_INCLUDED
	#define				CMOS_H_INCLUDED
	
	#define		CMOS		0x70
	#define		CMOSdata	0x71
	
	int CMOS_Floppy_Master;			//0=none 1=360KB 5.25 2=1.2MB 5.25 3=720KB 3.5 4=1.44MB 3.5 5=2.88MB 3.5
	int CMOS_Floppy_Slave;
	
	void readCMOS(void);
	const char *CMOS_Floppy_Decode(int type);
	
#endif
