#!/usr/bin/env bash

test_instr()
{
  echo -n "testing ${1} ... "

cat >${cpu}.asm << EOF
.${cpu}

  ${1}
EOF

  ../../naken_asm -o ${cpu}_naken.hex ${cpu}.asm > /dev/null

  a=`head -1 ${cpu}_naken.hex | tr -d '\n'`

  if [ "$a" = "${2}" ]
  then
    echo "PASS"
  else
    echo "FAIL ${a} ${b}"
    exit 0
  fi

  rm -f ${cpu}_naken.hex a.out ${cpu}.asm
}

cpu=${1}

echo "cpu=${cpu}"

while read line
do
  instr=`echo ${line} | awk -F\| '{ print $1 }'`
  hex=`echo ${line} | awk -F\| '{ print $2 }'`
  test_instr "${instr}" "${hex}"
done < ${cpu}.txt


