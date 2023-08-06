#!/usr/bin/env sh

a=`ldd ../../naken_asm | grep libstdc++`

if [[ ${a} != "" ]]
then
  echo "Error"
fi

a=`ldd ../../naken_util | grep libstdc++`

if [[ "${a}" != "" ]]
then
  echo "Error ${a}"
fi

