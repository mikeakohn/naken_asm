#!/usr/bin/env bash

ROOT=../../..

run_msp430_test()
{
  file=$1
  result=$2

  ${ROOT}/naken_asm -l -I ${ROOT}/include -o ${file}.hex ${file}.asm > /dev/null

  a=`${ROOT}/naken_util -run ${file}.hex`
  cycles=`echo "${a}" | grep cycles | tail -n 1 | sed 's/ clock cycles.*$//' | sed 's/^.* //'`
  answer=`echo ${a} | sed 's/^.* r15: //' | sed 's/,.*$//'`
  answer=`printf "%d" ${answer}`

  echo -n ${file} ": " ${cycles} "cycles", ${answer}

  if [ ${answer} -ne ${result} ]
  then
    echo " FAIL got ${answer} but expected ${result}"
    exit 1
  fi

  rm ${file}.lst
  rm ${file}.hex

  echo " PASS"
}

run_msp430_test immediate 165
run_msp430_test absolute 165
run_msp430_test symbolic 165
run_msp430_test symbolic_to_symbolic 165
run_msp430_test immediate_to_symbolic 165
run_msp430_test immediate_to_absolute 165
run_msp430_test immediate_to_indexed 165
run_msp430_test loop 10
run_msp430_test cg 24

