#!/usr/bin/env bash

PATH=/usbdisk/devkits/microchip/xc16/v1.11/bin:$PATH

cat /storage/git/naken_asm/testing/dspic/testing.asm | grep -v dspic > testing.asm

xc16-as testing.asm
xc16-objdump -d a.out

