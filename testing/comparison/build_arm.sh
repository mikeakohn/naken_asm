#!/usr/bin/env bash

test_instr()
{
  echo "building ${1} ... "

cat >arm.asm << EOF
.ifndef  __GNU__
.arm
.endif

  ${1}
EOF

  #cat arm.asm

  arm-linux-gnueabi-as arm.asm --defsym __GNU__=1
  arm-linux-gnueabi-objcopy -F ihex a.out arm_gnu.hex

  b=`head -1 arm_gnu.hex | tr -d '\r' | tr -d '\n'`

  echo "${1}|${b}" >> arm.txt

  rm -f arm_gnu.hex arm_naken.hex a.out arm.asm
}

echo -n > arm.txt

test_instr "add r0, r1, r2"
# Were these instructinos removed or what?
#test_instr "swp r0, r1, [r2]"
#test_instr "swi"
#test_instr "swilt"
test_instr "orr r5, r6, #256"



