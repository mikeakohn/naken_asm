#!/usr/bin/env bash

test_instr()
{
  echo -n "testing ${1} ... "

cat >avr8.asm << EOF
.ifndef  __GNU__
.avr8
.endif

  ${1}
EOF

  #cat avr8.asm

  avr-as avr8.asm -mmcu=atmega168 --defsym __GNU__=1
  avr-objcopy -F ihex a.out avr8_gnu.hex
  ../../naken_asm -o avr8_naken.hex avr8.asm > /dev/null

  a=`head -1 avr8_naken.hex | tr -d '\n'`
  b=`head -1 avr8_gnu.hex | tr -d '\r' | tr -d '\n'`

  if [ "$a" = "$b" ]
  then
    echo "PASS"
  else
    echo "FAIL ${a} ${b}"
    exit 0
  fi

  rm -f avr8_gnu.hex avr8_naken.hex a.out avr8.asm
}

test_instr "adc r5,r6"
test_instr "add r5,r6"
test_instr "and r5,r6"
test_instr "andi r19,10"
test_instr "adiw r24,10"
test_instr "asr r19"
test_instr "bclr 3"
test_instr "bld r19,3"
test_instr "test: brbc 1,test"
test_instr "test: brbs 1,test"
#test_instr "test: brcc 1,test"
#test_instr "test: brcs 1,test"
#test_instr "test: breq 1,test"
#test_instr "test: brge 1,test"
#test_instr "test: brhc 1,test"
#test_instr "test: brhs 1,test"
#test_instr "test: brid 1,test"
#test_instr "test: brie 1,test"
#test_instr "test: brlo 1,test"
#test_instr "test: brlt 1,test"
#test_instr "test: brmi 1,test"
#test_instr "test: brne 1,test"
#test_instr "test: brpl 1,test"
#test_instr "test: brsh 1,test"
#test_instr "test: brtc 1,test"
#test_instr "test: brts 1,test"
#test_instr "test: brvc 1,test"
test_instr "break"
test_instr "bset 3"
test_instr "bst r19,3"
test_instr "test: call test"
test_instr "cbr r19,3"
test_instr "clc"
test_instr "clr r24"
test_instr "clh"
test_instr "cli"
test_instr "cln"
test_instr "cls"
test_instr "clv"
test_instr "clt"
test_instr "clz"
test_instr "com r19"
test_instr "cp r24,r17"
test_instr "cpi r24,10"
test_instr "cpc r24,r17"
test_instr "cpse r24,r17"
test_instr "dec r19"
#test_instr "eicall"
#test_instr "eijmp"
test_instr "eor r19,r20"
test_instr "icall"
test_instr "ijmp"
test_instr "in r18, 10"
test_instr "inc r18"
test_instr "test: jmp test"
test_instr "ld r19, X"
test_instr "ld r19, -X"
test_instr "ld r19, X+"
test_instr "ldd r19, Y+10"
test_instr "ld r19, -Y"
test_instr "ld r19, Y+"
test_instr "ldd r19, Z+10"
test_instr "ld r19, -Z"
test_instr "ld r19, Z+"
test_instr "ldi r19, 100"
test_instr "lds r19, 100"
#test_instr "elpm"
#test_instr "elpm r19,Z"
#test_instr "elpm r19,Z+"
test_instr "lpm"
test_instr "lpm r19,Z"
test_instr "lpm r19,Z+"
test_instr "lsr r19"
#test_instr "mul r5,r6"
#test_instr "muls r5,r6"
#test_instr "mulsu r5,r6"
#test_instr "fmul r5,r6"
#test_instr "fmuls r5,r6"
#test_instr "fmulsu r5,r6"
test_instr "mov r5,r6"
test_instr "movw r16,r18"
test_instr "neg r19"
test_instr "or r19,r20"
test_instr "ori r24,10"
test_instr "out 10, r18"
test_instr "pop r18"
test_instr "push r18"
#test_instr "test: rcall test"
test_instr "ret"
test_instr "reti"
test_instr "ror r19"
#test_instr "test: rjmp test"
test_instr "sbci r18,10"
test_instr "sbiw r24,10"
test_instr "sbic 15,1"
test_instr "sbis 15,1"
test_instr "sbr r19,3"
test_instr "sbrc r24,1"
test_instr "sbrs r24,7"
test_instr "sec"
test_instr "seh"
test_instr "sei"
test_instr "sen"
test_instr "ses"
test_instr "set"
test_instr "sev"
test_instr "sez"
test_instr "ser r24"
test_instr "sleep"
test_instr "spm"
test_instr "st X, r18"
test_instr "st -X, r18"
test_instr "st X+, r18"
test_instr "st -Y, r18"
test_instr "st Y+, r18"
test_instr "st -Z, r18"
test_instr "st Z+, r18"
test_instr "std Y+10, r18"
test_instr "std Z+10, r18"
test_instr "sts 100, r18"
test_instr "sub r5,r6"
test_instr "swap r19"
test_instr "tst r19"
test_instr "wdr"




