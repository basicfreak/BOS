#!/bin/bash

sudo /sbin/losetup /dev/loop0 floppy
sudo mount /dev/loop0 /mnt2
sleep .25
sudo cp SRC/KERNEL.ELF /mnt2/KERNEL.ELF
sleep .25
sudo umount /dev/loop0
sudo /sbin/losetup -d /dev/loop0
