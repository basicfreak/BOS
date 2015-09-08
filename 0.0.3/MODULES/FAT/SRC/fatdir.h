#ifndef				_FATDIR_H_INCLUDED
	#define				_FATDIR_H_INCLUDED
	
	#include "fat.h"

	void ConvertFName(volatile char* output, const char* input);
	bool CompareFName(const char* s1, const char* s2);
	void SearchDir(uint32_t PartID, volatile PFATDIR MyDirEntry, char* FName);
	void CreateDir(uint32_t PartID, uint32_t ParrentCluster, char *DIRName);
	void addDirEnt(uint32_t PartID, PFATDIR DEnt, uint32_t ParrentCluster);
	void UpdateDirEntry(uint32_t PartID, PFATDIR DEnt, uint32_t ParrentCluster);
	void ReadRootDir(uint32_t PartID);
	void WriteRootDir(uint32_t PartID);

#endif