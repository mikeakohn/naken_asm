#!/usr/bin/env bash

test_instr()
{
  echo "building ${1} ..."

cat >msp430.asm << EOF
  ${1}
EOF

  #cat msp430.asm

  msp430-as msp430.asm -mmsp430x2553
  msp430-objcopy -F ihex a.out msp430_gnu.hex

  b=`head -1 msp430_gnu.hex | tr -d '\r' | tr -d '\n'`

  echo "${1}|${b}" >> msp430.txt

  rm -f msp430_gnu.hex msp430_naken.hex a.out msp430.asm
}

echo -n > msp430.txt

test_instr "setc"
test_instr "clrc"
test_instr "rrc.b r8"
test_instr "rrc.w r8"



