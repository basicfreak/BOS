#!/bin/bash

if [ "$1" = "mount" ]; then
	sudo losetup /dev/loop0 HDD.img -o $((63 * 512))
	sleep .10s
	sudo mount /dev/loop0 /mnt2
else
	if [ "$1" = "umount" ]; then
		sudo umount /mnt2
		sleep .10s
		sudo losetup -d /dev/loop0
	else
		echo "USSAGE:"
		echo -e "\t./hdd.sh mount"
		echo -e "\t./hdd.sh umount"	
	fi
fi