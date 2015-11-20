#!/bin/bash

sudo losetup /dev/loop0 HDD.img -o $((63 * 512))
sleep .10s
sudo mount /dev/loop0 /mnt2
sleep .10s

cd USERLAND/DLL/STDLIB
make clean
make
cd ../MEMLIB
make clean
make
cd ../PCILIB
make clean
make
cd ../../EXEC/ELF
make clean
make
cd ../TEST
make clean
make
cd ../VFS
#make clean
#make
cd ../../../x86/KERNEL
make
cd ../..
sudo cp USERLAND/EXEC/ELF/BIN/ELF.BIN /mnt2/bin/elf.bin
#sudo cp USERLAND/EXEC/VFS/BIN/VFS.ELF /mnt2/bin/vfs/.elf
sudo cp USERLAND/EXEC/TEST/BIN/TEST.ELF /mnt2/bin/test.elf
sudo cp USERLAND/DLL/STDLIB/BIN/STDLIB.DLL /mnt2/bin/stdlib.dll
sudo cp USERLAND/DLL/MEMLIB/BIN/MEMLIB.DLL /mnt2/bin/memlib.dll
sudo cp USERLAND/DLL/PCILIB/BIN/PCILIB.DLL /mnt2/bin/pcilib.dll
sudo cp x86/KERNEL/BIN/KERNEL.ELF /mnt2/bos/kernel.elf
sudo cp OSLOADER/boot.cfg /mnt2/boot/boot.cfg

sleep .10s
sudo umount /mnt2
sleep .10s
sudo losetup -d /dev/loop0
sleep .10s
