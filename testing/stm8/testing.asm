.stm8

main:
  break
  halt
  sim

  clrw x
  clrw y

  or A, #$55
  or A, $55
  or A, $1000
  or A, asdf
  xor A, (X)
  or A, (Y)

  and A, ($10, X)
  and A, ($1000, X)
  and A, ($10, Y)
  and A, ($1000, Y)
  and A, ($10, SP)
  and A, [$10]
  and A, [$1000]
  and A, ([$10],X)
  and A, ([$1000],X)
  and A, ([$10],Y)

asdf:

