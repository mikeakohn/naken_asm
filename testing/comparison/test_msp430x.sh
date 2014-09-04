#!/usr/bin/env bash

msp430-as msp430x.asm -mmsp430x2619 -mcpu=430x --defsym __GNU__=1
msp430-objcopy -F ihex a.out msp430x.hex

