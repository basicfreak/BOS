#!/bin/bash

echo "Building OSLOADER:"
cd OSLOADER
	echo -ne "\tAssembling FAT MBR..."
		nasm fat/mbr.s && echo "DONE!"
	echo -ne "\tAssembling FAT VBR..."
		nasm fat/vbr.s && echo "DONE!"
	echo -ne "\tAssembling FAT Loader..."
		nasm fat/osloader.s && echo "DONE!"
	echo -ne "\tAssembling PXE Loader..."
		nasm pxe/pxeloader.s && echo "DONE!"
echo "Building x86 KERNEL:"
cd ../x86/KERNEL
	make
cd ../..
echo "DONE!"
