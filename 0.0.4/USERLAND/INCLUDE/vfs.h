#ifndef _IO_H_INCLUDE
	#define _IO_H_INCLUDE

	#include <typedefines.h>

	typedef struct VFSDirectoryEntry dirent_t, *dirent_p;
	typedef struct VFSfnode f_node_t, *f_node_p;
	typedef struct VFSbnode b_node_t, *b_node_p;
	typedef struct FileSystems fs_t, *fs_p;

	struct VFSDirectoryEntry {
		char filename[124];
		node_p inode;
	} __attribute__((packed));

	struct VFSfnode {
		uint32_t flags;				// Includes the node type and permission mask
		uint32_t uid;				// The owning user.
		uint32_t gid;				// The owning group.
		uint32_t length;			// Size of the file, in bytes.

		uint32_t localnode;			// Local Variable for File System to find file
		b_node_p disk;				// Pointer to block device accociated with inode.

		f_node_p dirNode;

		f_node_p link;				// Used by mountpoints and symlinks.
	} __attribute__((packed));		// 32 bytes...

	typedef struct VFSbnode {
		uint32_t flags;				// Includes the node type and permission mask
		uint32_t uid;				// The owning user.
		uint32_t gid;				// The owning group.
		uint32_t length;			// Size of the file, in bytes.

		uint32_t start;				// Local Variable for File System to find file
		b_node_p disk;				// Pointer to block device accociated with inode.

		f_node_p dirNode;

		f_node_p link;				// Used by mountpoints and symlinks.
	} __attribute__((packed));

#endif