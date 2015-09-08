#!/bin/bash
C=gcc
AS=nasm
LD=ld
OUTPUT_FILE=KERNEL.ELF
CFLAGS="-m32  -fno-stack-protector -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes -std=gnu99 -nostdinc -fno-builtin -I./include -c -o"
AFLAGS="-f aout"
LDFLAGS="-T link.ld -o"

CFiles=( "./SRC/MAIN.c")
CObjs=( "./OBJ/MAIN.O")

AFiles=( "./SRC/START.ASM" )
AObjs=( "./OBJ/START.O" )

LDObjs="./OBJ/START.O ./OBJ/MAIN.O"

CCount=${#CFiles[@]}
ACount=${#AFiles[@]}

for (( i=0; i<$CCount; i++ ));
do
	$C $CFLAGS ${CObjs[$i]} ${CFiles[$i]}
done

for (( i=0; i<$ACount; i++ ));
do
	$AS $AFLAGS ${AFiles[$i]} -o ${AObjs[$i]}
done

$LD $LDFLAGS $OUTPUT_FILE $LDObjs

rm ./OBJ/*.O