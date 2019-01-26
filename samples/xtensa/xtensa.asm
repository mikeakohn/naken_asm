.xtensa

main:
  addi a5, a6, 9
  abs a3, a9
  abs.s f3, f9
  add a5, a6, a9
  add.n a5, a6, a9
  add.s f9, f15, f2
  addi.n a9, a15, -1
  addi.n a9, a15, 9
  addmi a5, a6, -32768
  addx2 a5, a6, a9
  addx4 a5, a6, a9
  addx8 a5, a6, a9
  all4 b3, b4
here:
  all8 b3, b8
  andb b3, b4, b5
  ball a4, a5, here
  beqi a4, 128, here
  beqz a4, here
  beqz.n a4, here

