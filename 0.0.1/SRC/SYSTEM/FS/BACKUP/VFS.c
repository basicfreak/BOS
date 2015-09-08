/*
./LIBSRC/FS/VFS.C
*/

#include "VFS.H"

#define DEVICE_MAX 26
PFILESYSTEM _FileSystems[DEVICE_MAX];

FILE VFS_OpenFile (const char* fname)
{
	if (fname) {
		// default to device 'a'
		unsigned char device = 'a';
		// filename
		char* filename = (char*) fname;
		// in all cases, if fname[1]==':' then the first character must be device letter
		if (fname[1]==':') {

			device = (unsigned char) fname[0];
			filename += 3; //strip it from pathname
		}

		// call filesystem
		if (_FileSystems [device - 'a']) {
			// set volume specific information and return file
			FILE file = _FileSystems[device - 'a']->Open (filename);
			file.deviceID = device;
			return file;
		}
	}
	// return invalid file
	FILE file;
	file.flags = FS_INVALID;
	return file;
}

void VFS_ReadFile (PFILE file, unsigned char* Buffer, unsigned int Length)
{
	if (file)
		if (_FileSystems [file->deviceID - 'a'])
			_FileSystems[file->deviceID - 'a']->Read (file,Buffer,Length);
}

void VFS_CloseFile (PFILE file)
{
	if (file)
		if (_FileSystems [file->deviceID - 'a'])
			_FileSystems[file->deviceID - 'a']->Close (file);
}

void VFS_RegisterFileSystem (PFILESYSTEM fsys, unsigned int deviceID)
{
	static int i=0;

	if (i < DEVICE_MAX)
		if (fsys) {
			_FileSystems[ deviceID ] = fsys;
			i++;
		}
}

void VFS_UnregisterFileSystem (PFILESYSTEM fsys)
{
	for (int i=0;i < DEVICE_MAX; i++)
		if (_FileSystems[i]==fsys)
			_FileSystems[i]=0;
}

void VFS_UnregisterFileSystemByID (unsigned int deviceID)
{
	if (deviceID < DEVICE_MAX)
		_FileSystems [deviceID] = 0;
}
