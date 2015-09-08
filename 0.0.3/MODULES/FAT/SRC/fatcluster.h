#ifndef				_FATCLUSTER_H_INCLUDED
	#define				_FATCLUSTER_H_INCLUDED
	
	#include <api.h>

	void ReadCluster(uint32_t PartID, uint32_t CurrentCluster, void* Buffer);
	void WriteCluster(uint32_t PartID, uint32_t CurrentCluster, void* Buffer);
	uint32_t NextCluster(uint32_t PartID, uint32_t CurrentCluster);
	uint32_t CountCluster(uint32_t PartID, uint32_t CurrentCluster);
	uint32_t CountClusterChain(uint32_t PartID, uint32_t CurrentCluster);
	void ReadClusterChain(uint32_t PartID, uint32_t CurrentCluster, void* Buffer, uint32_t Count);
	uint32_t FindFreeCluster(uint32_t PartID);
	void AllocateCluster(uint32_t PartID, uint32_t CurrentCluster);
	void LinkCluster(uint32_t PartID, uint32_t Cluster0, uint32_t Cluster1);
	void ReadFAT(uint32_t PartID);
	void WriteFAT(uint32_t PartID);

#endif