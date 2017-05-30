#!/usr/bin/env bash

test_instr()
{
  echo -n "[${3}]testing ${1} ... "
  extra=""

  if [ "${cpu}" == "epiphany" ]
  then
    extra='.include "../../include/epiphany/epiphany.inc"'
  fi

  if [ "${cpu}" == "lc3" ]
  then
    extra='.dc16 0'
  fi

cat >${cpu}.asm << EOF
.${cpu}
${extra}
start:
  ${1}
EOF

  #if [ "${cpu}" == "mips32" ] && [ "${1:0:1}" == "b" ]
  #then
  #  echo " nop" >> ${cpu}.asm
  #fi

  #if [ "${cpu}" == "mips32" ] && [ "${1:0:1}" == "j" ]
  #then
  #  echo " nop" >> ${cpu}.asm
  #fi

  if [ "${cpu}" == "pic32" ] && [ "${1:0:5}" == "main:" ]
  then
    echo " nop" >> ${cpu}.asm
  fi

  if [ "${cpu}" == "pic32" ] && [ "${1:0:1}" == "j" ]
  then
    echo " nop" >> ${cpu}.asm
  fi

  if [ "${cpu}" == "ps2_ee" ] && [ "${1:0:5}" == "main:" ]
  then
    echo " nop" >> ${cpu}.asm
  fi

  if [ "${cpu}" == "ps2_ee" ] && [ "${1:0:1}" == "j" ]
  then
    echo " nop" >> ${cpu}.asm
  fi

  ../../naken_asm -o ${cpu}_naken.hex ${cpu}.asm > /dev/null

  a=`head -1 ${cpu}_naken.hex | tr -d '\n'`

  if [ "$a" = "${2}" ]
  then
    echo -e "\x1b[32mPASS\x1b[0m"
  else
    echo -e "\x1b[31mFAIL ${a} ${b} -> ${2}\x1b[0m"
    exit 1
  fi

  rm -f ${cpu}_naken.hex a.out ${cpu}.asm
}

cpu=${1}

echo "cpu=${cpu}"

while read line
do
  instr=`echo ${line} | awk -F\| '{ print $1 }'`
  hex=`echo ${line} | awk -F\| '{ print $2 }'`
  test_instr "${instr}" "${hex}" "${cpu}"
done < ${cpu}.txt

