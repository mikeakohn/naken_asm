#!/usr/bin/env bash

test_instr()
{
  echo -n "testing ${1} ... "

cat >msp430.asm << EOF
.ifndef  __GNU__
.msp430
.endif

  ${1}
EOF

  #cat msp430.asm

  msp430-as msp430.asm -mmsp430x2553 -mcpu=430x --defsym __GNU__=1
  msp430-objcopy -F ihex a.out msp430_gnu.hex
  ../../naken_asm -o msp430_naken.hex msp430.asm > /dev/null

  a=`head -1 msp430_naken.hex | tr -d '\n'`
  b=`head -1 msp430_gnu.hex | tr -d '\r' | tr -d '\n'`

  if [ "$a" = "$b" ]
  then
    echo "PASS"
  else
    echo "FAIL ${a} ${b}"
    exit 0
  fi

  rm -f msp430_gnu.hex msp430_naken.hex a.out msp430.asm
}

test_instr "setc"
test_instr "clrc"
test_instr "rrc.b r8"
test_instr "rrc.w r8"



