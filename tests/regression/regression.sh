#!/usr/bin/env bash

test_arch()
{
  b=`../../naken_asm -o out.hex ../../samples/$1/testing.asm`

  if [ $? -ne 0 ]
  then
    echo "Failed $1 ... (assembler error)"
    exit -1
  fi

  a=`diff out.hex $1.hex`

  if [ "${a}" != "" ]
  then
    echo "Failed $1 ..."
  else
    echo "Passed $1 ..."
  fi

  rm -f out.hex out.lst
}

test_arch "8051"
#test_arch "arm"
test_arch "avr8"
test_arch "dspic"
test_arch "msp430"
test_arch "stm8"
test_arch "tms9900"
test_arch "z80"

