#!/bin/bash

sudo losetup /dev/loop0 HDD.img -o $((63 * 512))
sleep .10s
sudo mount /dev/loop0 /mnt2
sleep .10s

cd MODULES/SVGA
make
cd ../CLI
make
cd ../../KERNEL
make
cd ..
sudo cp MODULES/CLI/BIN/CLI.DLL /mnt2/bin/cli.dll
sudo cp MODULES/SVGA/BIN/SVGA.DLL /mnt2/bin/svga.dll
sudo cp KERNEL/BIN/KERNEL.ELF /mnt2/bin/kernel.elf

sleep .10s
sudo umount /dev/loop0
sleep .10s
sudo losetup -d /dev/loop0
sleep .10s
