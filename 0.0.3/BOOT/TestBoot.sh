#!/bin/bash

echo "BOS v. 0.0.2 Test Boot SCRIPT (c) 2015 Brian T. Hoover"

# Variables
	DISKIMAGE="BOOTSECT"
	LOOPBACKDEVICE="/dev/loop0"
	SLEEPTIME=".10s"
# Programs and Commands
	BOCHSbin="/usr/bin/bochs"
	CDbin="cd"
	CPbin="/bin/cp"
	LOSETUPbin="/sbin/losetup"
	MOUNTbin="/bin/mount"
	SLEEPbin="/bin/sleep"
	SUDObin="/usr/bin/sudo"
	UMOUNTbin="/bin/umount"
	UPDIR=".."
	NASMbin="/usr/bin/nasm"

echo "Compiling STAGE ONE"

$NASMbin STAGE1/BOOTSECT.ASM -o $DISKIMAGE

echo "Done!"

echo "Mounting Disk Image..."

$SUDObin $LOSETUPbin $LOOPBACKDEVICE $DISKIMAGE
$SLEEPbin $SLEEPTIME
$SUDObin $MOUNTbin $LOOPBACKDEVICE /mnt2
$SLEEPbin $SLEEPTIME

echo "Done!"

echo "Compiling STAGE TWO"

$SUDObin $NASMbin STAGETWO/STAGETWO.ASM -o /mnt2/STAGETWO.BIN

echo "Done!"

echo "Un-Mounting Disk Image..."
$SUDObin mkdir /mnt2/boot
$SUDObin mkdir /mnt2/bin
$SUDObin $CPbin BOOT.CFG /mnt2/boot/
cd ../KERNEL
make clean
make
cd ../BOOT
$SUDObin $CPbin ../KERNEL/BIN/KERNEL.ELF /mnt2/bin/
$SUDObin $CPbin ../MODULES_BAK/SVGA/BIN/SVGA.DLL /mnt2/bin/

$SLEEPbin $SLEEPTIME
$SUDObin $UMOUNTbin $LOOPBACKDEVICE
$SLEEPbin $SLEEPTIME
$SUDObin $LOSETUPbin -d $LOOPBACKDEVICE
$SLEEPbin $SLEEPTIME

echo "Done!"

echo "Running Bochs..."

$SUDObin $BOCHSbin

echo "Done!"

echo "BOS v. 0.0.2 Test Boot SCRIPT (c) 2015 Brian T. Hoover"