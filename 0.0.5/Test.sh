#!/bin/bash

cd BOOT/SYSINIT
make
cd ../DRIVERS/FATFS
make
cd ../../X86/BUILD
make
cd ../../X64/BUILD
make
cd ../../..
sudo mount -ooffset=32256 hdd.img hdd_mount
sudo cp BOOT/SYSINIT.BIN hdd_mount/boot/sysinit.bin
sudo cp BOOT/DRIVERS/FATFS.BIN hdd_mount/boot/drivers/fatfs.bin
sudo cp BOOT/X86/BUILD.BIN hdd_mount/boot/x86/build.bin
sudo cp BOOT/X64/BUILD.BIN hdd_mount/boot/x64/build.bin
sync
sudo umount hdd_mount

