#!/bin/bash

echo "BOS v. 0.0.2 Clean-Make SCRIPT (c) 2015 Brian T. Hoover"

#Space Separated Arrays:
	# Modules to Make and Copy
	MODULElist="DMA FDC FAT PCI MBR IDE SVGA CLI"	
	# Libraries to Make and Copy
	LIBRARYlist=""

# Variables
	KERNELPath="KERNEL"
	LIBRARYPath="LIBRARIES"
	MODULEPath="MODULES"
	
# Programs and Commands
	CDbin="cd"
	MAKEbin="/usr/bin/make"
	UPDIR=".."

echo "Cleaning Kernel..."

$CDbin $KERNELPath
$MAKEbin clean

echo "Done!"

echo "Building Kernel..."

$MAKEbin
$CDbin $UPDIR

echo "Done!"

echo "Cleaning and Building Modules..."

$CDbin $MODULEPath
for Mod in $MODULElist
do
	echo "    Cleaning Module \"$Mod\"..."
	$CDbin $Mod
	$MAKEbin clean

	echo "    Done!"

	echo "    Building Module \"$Mod\"..."

	$MAKEbin
	$CDbin $UPDIR

	echo "    Done!"
done
$CDbin $UPDIR

echo "Done!"

echo "Cleaning and Building Libraries..."

$CDbin $LIBRARYPath
for Lib in $LIBRARYlist
do
	echo "    Cleaning Library \"$Lib\"..."
	$CDbin $Lib
	$MAKEbin clean

	echo "    Done!"

	echo "    Building Library \"$Lib\"..."

	$MAKEbin
	$CDbin $UPDIR

	echo "    Done!"
done
$CDbin $UPDIR

echo "Done!"

echo "BOS v. 0.0.2 Clean-Make SCRIPT (c) 2015 Brian T. Hoover"
