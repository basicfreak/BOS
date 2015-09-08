#!/bin/bash

echo "BOS v. 0.0.2 Make-Update-Run SCRIPT (c) 2015 Brian T. Hoover"

#Space Separated Arrays:
	# Modules to Make and Copy
	MODULElist="DMA FDC FAT PCI MBR IDE SVGA CLI"	
	# Libraries to Make and Copy
	LIBRARYlist=""

# Variables
	BINSubDir="BIN"
	DISKIMAGE="floppy.img"
	BOOTMenu="BOOT.CFG"
	KERNELPath="KERNEL"
	LIBRARYPath="LIBRARIES"
	LOOPBACKDEVICE="/dev/loop0"
	MODULEPath="MODULES"
	SLEEPTIME=".10s"

# Programs and Commands
	BOCHSbin="/usr/bin/bochs"
	CDbin="cd"
	CPbin="/bin/cp"
	LOSETUPbin="/sbin/losetup"
	MAKEbin="/usr/bin/make"
	MOUNTbin="/bin/mount"
	SLEEPbin="/bin/sleep"
	SUDObin="/usr/bin/sudo"
	UMOUNTbin="/bin/umount"
	UPDIR=".."

$CDbin $KERNELPath

echo "Building Kernel..."

$MAKEbin
$CDbin $UPDIR

echo "Done!"

echo "Building Modules..."

$CDbin $MODULEPath
for Mod in $MODULElist
do
	$CDbin $Mod
	
	echo "    Building Module \"$Mod\"..."

	$MAKEbin
	$CDbin $UPDIR

	echo "    Done!"
done
$CDbin $UPDIR

echo "Done!"

echo "Building Libraries..."

$CDbin $LIBRARYPath
for Lib in $LIBRARYlist
do
	$CDbin $Lib
	
	echo "    Building Library \"$Lib\"..."

	$MAKEbin
	$CDbin $UPDIR

	echo "    Done!"
done
$CDbin $UPDIR

echo "Done!"

echo "Mounting Disk Image..."

$SUDObin $LOSETUPbin $LOOPBACKDEVICE $DISKIMAGE
$SLEEPbin $SLEEPTIME
$SUDObin $MOUNTbin $LOOPBACKDEVICE /mnt2
$SLEEPbin $SLEEPTIME

echo "Done!"

echo "Copying BOOT Menu To Disk Image..."

$SUDObin $CPbin $BOOTMenu /mnt2/BOOT/BOOT.CFG

echo "Done!"

echo "Copying Kernel To Disk Image..."


$SUDObin $CPbin $KERNELPath/$BINSubDir/* /mnt2/$BINSubDir/

echo "Done!"

echo "Copying Modules To Disk Image..."

$CDbin $MODULEPath
for Mod in $MODULElist
do
	echo "    Copying Module \"$Mod\"..."
	$CDbin $Mod/$BINSubDir
	$SUDObin $CPbin * /mnt2/$BINSubDir/

	echo "    Done!"

	$CDbin $UPDIR/$UPDIR
done
$CDbin $UPDIR

echo "Done!"

echo "Copying Libraries To Disk Image..."


$CDbin $LIBRARYPath
for Lib in $LIBRARYlist
do
	echo "    Copying Library \"$Lib\"..."
	$CDbin $Lib/$BINSubDir
	$SUDObin $CPbin * /mnt2/$BINSubDir/

	echo "    Done!"

	$CDbin $UPDIR/$UPDIR
done
$CDbin $UPDIR

echo "Done!"

echo "Un-Mounting Disk Image..."

$SLEEPbin $SLEEPTIME
$SUDObin $UMOUNTbin $LOOPBACKDEVICE
$SLEEPbin $SLEEPTIME
$SUDObin $LOSETUPbin -d $LOOPBACKDEVICE
$SLEEPbin $SLEEPTIME

echo "Done!"

echo "Running Bochs..."

$SUDObin $BOCHSbin

echo "Done!"

echo "BOS v. 0.0.2 Make-Update-Run SCRIPT (c) 2015 Brian T. Hoover"
