#!/bin/bash

sudo losetup /dev/loop0 HDD.img -o $((63 * 512))
sleep .10s
sudo mount /dev/loop0 /mnt2
sleep .10s
sudo cp BOOT16/OSLOADER/osloader /mnt2/osloader.sys 
sleep .10s
sudo umount /dev/loop0
sleep .10s
sudo losetup -d /dev/loop0
sleep .10s
