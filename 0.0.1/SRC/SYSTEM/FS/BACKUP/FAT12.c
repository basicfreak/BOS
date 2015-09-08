/*
./LIBSRC/FS/FAT12.C
*/

#include "FAT12.H"
#include "BPB.H"
#include <STRING.H>
#include <STDIO.H>
#include "../../HARDWARE/FDC.H"
#include "../MEM/PHYSICAL.H"
#include <MATH.H>

///#define debug 0

#define SECTOR_SIZE 512
FILESYSTEM _FSysFat;
MOUNT_INFO _MountInfo;
uint16_t FATBlock [(SECTOR_SIZE * 9 * 8)/12];
RDIRS FATRDIR;
PRDIR _ROOTdir;
RDIRS TempDir;
PRDIR _CurDir;
uint8_t dataBuffer[0x2400] = { 0 };


void ToDosFileName (const char* filename, char* fname, unsigned int FNameLength);
void FATBlock_init(void);
uint16_t Cluster(uint16_t cluster);
void ROOTdir_init(void);
FILE SearchSubDirectory(FILE subDir, const char* FName);
void fsysFatClose (PFILE file);


void FATBlock_init()
{
	uint8_t* data = (uint8_t*) &dataBuffer;
	memset(data, 0, sizeof(dataBuffer));
	_FDC_IO(false, 1, 9, data);
	for (int i = 0; i < (SECTOR_SIZE * 9 * 8)/12; i++) {
		uint16_t clust = (uint16_t) (i * 1.5);
		uint16_t value = 0;
		value = (uint16_t) data[clust+1];
		value = (uint16_t) ((value * 256) + data[clust]);
		if(isEven((uint64_t) i))
			FATBlock[i] = value & 0x0FFF;
		else
			FATBlock[i] = (value >> 4);
	}
}

uint16_t Cluster(uint16_t cluster)
{
	return FATBlock[cluster];
}

void ROOTdir_init()
{
	PRDIR* data = (PRDIR*) &dataBuffer;
	memset(data, 0, sizeof(dataBuffer));
	_FDC_IO(false, 19, 14, data);
	memcpy(&FATRDIR, data, SECTOR_SIZE*14);
	for (int i = 0; i < 224; i++) {
		_ROOTdir->entry[i] = &(FATRDIR.entry[i]);
	}
}

void dirTest()
{
	for (int i = 0, j = 0; i < 224; i++) {
		//printf("%i\t", i);
		PDIRECTORY temp = _ROOTdir->entry[i];
		//for(int lol = 0; lol < 11; lol++)
		//	name[lol] = temp.Filename[lol];
		///char name[11];
		///memcpy(name, temp->Filename, 11);
		///name[11]='\0';
		if(temp->Filename[0] != 0xE5 && temp->Filename[0] != 0 && strlen((const char*) temp->Filename) >= 7) {
			char name[13];
			name[0] = (char) temp->Filename[0];
			name[1] = (char) temp->Filename[1];
			name[2] = (char) temp->Filename[2];
			name[3] = (char) temp->Filename[3];
			name[4] = (char) temp->Filename[4];
			name[5] = (char) temp->Filename[5];
			name[6] = (char) temp->Filename[6];
			name[7] = (char) temp->Filename[7];
			name[8] = '.';
			name[9] = (char) temp->Ext[0];
			name[10] = (char) temp->Ext[1];
			name[11] = (char) temp->Ext[2];
			name[12] = 0;
			if(name[0] == 0x5E)
				name[0] = (char) 0xE5;
			if(temp->Attrib&0x10) printfc(0x1, "%s", name);
			else printf("%s", name);///printf("%i.\t%s\t0x%x\n",j,name,temp->FirstCluster);
			j++;
			if (j%5==0) putch('\n');
			else putch('\t');
		}
		if(!temp->Filename[0]) break;
	}
	putch('\n');
}

FILE SearchSubDirectory(FILE subDir, const char* FName)
{
	FILE ret;
	ret.flags = FS_INVALID;
	
	char DosFileName[12];
	ToDosFileName (FName, DosFileName, 11);
	DosFileName[11]=0;
	
	unsigned char data[0x200];
	while (subDir.eof != 1) {
		fsysFatRead ( &subDir, data, 0x200);
		for (int i = 0; i < 0x10; i++) {		//search section of directory for FName 0x200/0x20=0x10
			PDIRECTORY temp = (PDIRECTORY) &data[i*0x20];
			char name[12];
			memcpy (name, temp->Filename, 11);
			name[11]=0;
			if (streql (DosFileName, name)) {
				// found it, set up file info
				strcpy (ret.name, FName);
				ret.id             = 0;
				ret.currentCluster = temp->FirstCluster;
				ret.fileLength     = temp->FileSize;
				ret.eof            = 0;
				ret.fileLength     = temp->FileSize;
				// set file type
				if (temp->Attrib == 0x10)
					ret.flags = FS_DIRECTORY;
				else
					ret.flags = FS_FILE;
				// return file
				return ret;
			}
		}
	}
	return ret;
}

void ToDosFileName (const char* filename, char* fname, unsigned int FNameLength)
{
	unsigned int i=0;
	if (FNameLength > 11)
		return;
	if (!fname || !filename)
		return;
	// set all characters in output name to spaces
	memset (fname, ' ', (size_t) FNameLength);
	// 8.3 filename
	for (i=0; i < (unsigned int) (strlen(filename)-1) && i < (unsigned int) FNameLength; i++) {
		if (filename[i] == '.' || i==8 )
			break;
		// capitalize character and copy it over (we dont handle LFN format)
		fname[i] = ChartoUpper (filename[i] );
	}
	// add extension if needed
	if (filename[i]=='.') {
		// note: cant just copy over-extension might not be 3 chars
		for (int k=0; k<3; k++) {
			++i;
			if ( filename[i] )
				fname[8+k] = filename[i];
		}
	}
	// extension must be uppercase (we dont handle LFNs)
	for (i = 0; i < 3; i++)
		fname[8+i] = ChartoUpper (fname[8+i]);
}

FILE fsysFatDirectory (const char* DirectoryName)
{
	FILE file;
	// get 8.3 directory name
	char DosFileName[12];
	ToDosFileName (DirectoryName, DosFileName, 11);
	DosFileName[11]=0;
	// get current filename
	for (int i = 0; i < 224; i++) {
		PDIRECTORY directory = _ROOTdir->entry[i];
		char name[12];
		memcpy (name, directory->Filename, 11);
		name[11]=0;
		// find a match?
		if (streql (DosFileName, name)) {
			// found it, set up file info
			strcpy (file.name, DirectoryName);
			file.id             = 0;
			file.currentCluster = directory->FirstCluster;
			file.fileLength     = directory->FileSize;
			file.eof            = 0;
			file.fileLength     = directory->FileSize;
			// set file type
			if (directory->Attrib == 0x10)
				file.flags = FS_DIRECTORY;
			else
				file.flags = FS_FILE;
			// return file
			return file;
		}
		// go to next directory
	}
	// unable to find file
	file.flags = FS_INVALID;
	return file;
}

void fsysFatRead(PFILE file, unsigned char* Buffer, unsigned int Length)
{
	if (file) {
		uint8_t* sector = (uint8_t*) &dataBuffer;
		memset(sector, 0, sizeof(dataBuffer));
		_FDC_IO(false, (int) (31 + file->currentCluster), (uint8_t) (Length / 0x200), sector);
		// copy block of memory
		memcpy (Buffer, sector, (size_t)Length);
		// read entry for next cluster
		uint16_t nextCluster = Cluster((uint16_t) file->currentCluster);
		if ( !nextCluster || nextCluster >= 0xff8)
			file->eof = 1;
		else	// set next cluster
			file->currentCluster = nextCluster;
	}
}

void fsysFatClose (PFILE file)
{
	if (file)
		file->flags = FS_INVALID;
}

FILE fsysFatOpen (const char* FileName)
{
	FILE curDirectory;
	char* p = 0;
	char* path = (char*) FileName;
	// any '\'s in path?
	p = strchr (path, '\\');
	if (!p) {
		// nope, must be in root directory, search it
		curDirectory = fsysFatDirectory (path);
		// found file?
		if (curDirectory.flags == FS_FILE)
			return curDirectory;
	}
	else {
		char out[50][100];
		int temp = explode( out, (const char *)path, '\\' );
		for (int z = 0; z < temp; z ++) {
			if (!z) { // z = 0 looking for first sub dir
				curDirectory = fsysFatDirectory (out[z]);
				// found file?
				if (curDirectory.flags != FS_DIRECTORY) { //subdirectory NOT found
					FILE ret;
					ret.flags = FS_INVALID;
					return ret;
				}
			} else {
				curDirectory =  SearchSubDirectory(curDirectory, out[z]);
				//curDirectory = fsysFatDirectory (out[z]);
				if (curDirectory.flags != FS_DIRECTORY) { //subdirectory NOT found
					FILE ret;
					ret.flags = FS_INVALID;
					return ret;
				}
			}
		}
		curDirectory =  SearchSubDirectory(curDirectory, out[temp]);
		if (curDirectory.flags == FS_FILE)
			return curDirectory;
	}
	// unable to find
	FILE ret;
	ret.flags = FS_INVALID;
	return ret;
}

void fsysFatInitialize ()
{
	// initialize filesystem struct
	strcpy (_FSysFat.Name, "FAT12");
	_FSysFat.Directory = fsysFatDirectory;
	_FSysFat.Mount     = fsysFatMount;
	_FSysFat.Open      = fsysFatOpen;
	_FSysFat.Read      = fsysFatRead;
	_FSysFat.Close     = fsysFatClose;
	// register ourself to volume manager
	VFS_RegisterFileSystem ( &_FSysFat, 0 );
	// mount filesystem
	fsysFatMount ();
	FATBlock_init();
	ROOTdir_init();
}

void fsysFatMount ()
{
	// Boot sector info
	PBOOTSECTOR bootsector = (PBOOTSECTOR) &dataBuffer;
	memset(bootsector, 0, sizeof(dataBuffer));
	// Read Boot Sector
	_FDC_IO(false, 0, 1, bootsector);
	// store mount info
	_MountInfo.numSectors     = bootsector->Bpb.NumSectors;
	_MountInfo.fatOffset      = 1;
	_MountInfo.fatSize        = bootsector->Bpb.SectorsPerFat;
	_MountInfo.fatEntrySize   = 8;
	_MountInfo.numRootEntries = bootsector->Bpb.NumDirEntries;
	_MountInfo.rootOffset     = (uint32_t) ((bootsector->Bpb.NumberOfFats * bootsector->Bpb.SectorsPerFat) + 1);
	_MountInfo.rootSize       = (uint32_t) (( bootsector->Bpb.NumDirEntries * 32 ) / bootsector->Bpb.BytesPerSector);
}
