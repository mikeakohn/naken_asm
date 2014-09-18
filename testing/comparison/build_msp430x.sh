#!/usr/bin/env bash

test_instr()
{
  echo "building ${1} ... "

cat >msp430x.asm << EOF
.ifndef  __GNU__
.msp430x
.endif

  ${1}
EOF

  #cat msp430x.asm

  msp430-as msp430x.asm -mmsp430x2619 -mcpu=430x --defsym __GNU__=1
  msp430-objcopy -F ihex a.out msp430x_gnu.hex

  b=`head -1 msp430x_gnu.hex | tr -d '\r' | tr -d '\n'`

  echo "${1}|${b}" >> msp430x.txt

  rm -f msp430x_gnu.hex msp430x_naken.hex a.out msp430x.asm
}

echo -n > msp430x.txt

test_instr "pushm #1, r5"
test_instr "pushm.w #1, r5"
test_instr "pushm.a #1, r5"
test_instr "rrcm.w #3, r5"
test_instr "rrcm.a #3, r5"
test_instr "rrcm #3, r5"
test_instr "rram #3, r7"
test_instr "rlam.w #1, r9"
test_instr "rrum.a #2, r10"
test_instr "mova @r10, r11"
test_instr "mova @r10+, r11"
test_instr "mova &0x12344, r11"
test_instr "mova 5(r10), r11"
test_instr "mova r11, &0x12344"
test_instr "mova r11, 5(r10)"
test_instr "mova #0x12345, r10"
test_instr "cmpa #0x12345, r10"
test_instr "adda #0x12345, r10"
test_instr "suba #0x12345, r10"
test_instr "mova r11, r10"
test_instr "calla &0x12346"
test_instr "calla 10(r9)"
test_instr "calla 10(r0)"
#test_instr "calla 0x12346"
test_instr "calla #0x12346"
test_instr "movx r5,r6"



