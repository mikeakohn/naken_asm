#!/usr/bin/env bash

test_compare()
{
#a=`arm-linux-gnueabi-as -version`
a=`$1 -version`

if [ $? -eq 0 ]
then
  echo "Testing $2"
  #echo `sh test_$2.sh`
  sh test_$2.sh
else
  echo "Can't find $1"
fi

}

# sudo apt-get install gcc-arm-linux-gnueabi
# sudo apt-get install gcc-msp430

test_compare "arm-linux-gnueabi-as" "arm"
test_compare "msp430-as" "msp430x"
test_compare "avr-as" "avr8"

echo "Compare test done"

