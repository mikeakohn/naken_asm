#!/usr/bin/env bash

PATH=/usbdisk/devkits/microchip/xc16/v1.11/bin:$PATH

test_instr()
{
  echo "building ${1} ..."

cat >dspic.asm << EOF

  ${1}
EOF

  xc16-as dspic.asm
  xc16-bin2hex a.out
  #xc16-objcopy -F ihex a.out dspic_gnu.hex
  #xc16-objdump -d a.out
  b=`egrep '^:04' a.hex | tr '[:lower:]' '[:upper:]' | tr -d '\r' | tr -d '\n'`

  echo "${1}|${b}" >> dspic.txt

  #rm -f a.hex a.out dspic.asm
}

echo -n > dspic.txt

test_instr "push 0x1234"


