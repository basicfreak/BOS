/*
./INCLUDE/FS/VFS.H
*/

#ifndef				VFS_H_INCLUDED
	#define				VFS_H_INCLUDED

	#include <FORMATTING.H>

	typedef struct _FILE {

		char        name[32];
		uint32_t    flags;
		uint32_t    fileLength;
		uint32_t    id;
		uint32_t    eof;
		uint32_t    position;
		uint32_t    currentCluster;
		uint32_t    deviceID;

	}FILE, *PFILE;

	typedef struct _FILE_SYSTEM {

		char Name [8];
		FILE               (*Directory)  (const char* DirectoryName);
		void	           (*Mount)      (void);
		void               (*Read)       (PFILE file, unsigned char* Buffer, unsigned int Length);
		void	           (*Close)      (PFILE);
		FILE               (*Open)       (const char* FileName);

	}FILESYSTEM, *PFILESYSTEM;

	#define FS_FILE       0
	#define FS_DIRECTORY  1
	#define FS_INVALID    2
	
	FILE VFS_OpenFile (const char* fname);
	void VFS_ReadFile (PFILE file, unsigned char* Buffer, unsigned int Length);
	void VFS_CloseFile (PFILE file);
	void VFS_RegisterFileSystem (PFILESYSTEM fsys, unsigned int deviceID);
	void VFS_UnregisterFileSystem (PFILESYSTEM fsys);
	void VFS_UnregisterFileSystemByID (unsigned int deviceID);


#endif