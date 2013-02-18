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
  bset $1000, #1
  bres $1000, #2
  bcpl $1000, #3
  bccm $1000, #4
  btjt $1000, #5, asdf
  btjf $1000, #6, asdf
  jrc asdf

  mul x,a
  mul y,a
  div x,a
  div y,a

  divw x,y
  exgw x,y

  swap a
  swap (x)
  swap $10
  swap $1000
  swap ($10,x)
  swap ($1000,x)
  swap ($10,y)
  swap ($1000,y)
  swap ($10,sp)
  swap [$10]
  swap [$1000]
  swap ([$10], X)
  swap ([$1000], X)
  swap ([$10], Y)

  wfe
  push a
  push cc
  push #$10
  push $1000

boogers:
  pop a
  pop cc
  pop $1000
  jra boogers
  jrt boogers
  jrf boogers
  jrc boogers
  callr boogers
  call $1100

  exg a, xl
  exg a, yl
  exg a, $1132

  callf $123456
  callf [$1234]
  jpf $123456
  jpf [$1234]

  cpw x, #$10
  cpw x, $10
  cpw x, $1000
  cpw x, (Y)
  cpw x, ($10, Y)
  cpw x, ($1000, Y)
  cpw x, ($10, SP)
  cpw x, [$10]
  cpw x, [$1000]
  cpw x, ([$10], Y)

  mov $8000, #$aa
  mov $80, $10
  mov $8000, $1000

