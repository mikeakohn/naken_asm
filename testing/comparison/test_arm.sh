#!/usr/bin/env bash

test_instr()
{
  echo -n "testing ${1} ... "

cat >arm.asm << EOF
.ifndef  __GNU__
.arm
.endif

  ${1}
EOF

  #cat arm.asm

  arm-linux-gnueabi-as arm.asm --defsym __GNU__=1
  arm-linux-gnueabi-objcopy -F ihex a.out arm_gnu.hex
  ../../naken_asm -o arm_naken.hex arm.asm > /dev/null

  a=`head -1 arm_naken.hex | tr -d '\n'`
  b=`head -1 arm_gnu.hex | tr -d '\r' | tr -d '\n'`

  if [ "$a" = "$b" ]
  then
    echo "PASS"
  else
    echo "FAIL ${a} ${b}"
    #exit 0
  fi

  rm -f arm_gnu.hex arm_naken.hex a.out arm.asm
}

test_instr "add r0, r1, r2"
# Were these instructinos removed or what?
#test_instr "swp r0, r1, [r2]"
#test_instr "swi"
#test_instr "swilt"
test_instr "orr r5, r6, #256"



