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
  bf b3, here
  break 3, 7
  break.n 12 
  call0 here
  callx0 a5
  ceil.s a5, f8, 7
  clamps a7, a9, 15
  dsync
  dii a5, 1020
  dpfl a13, 240
  dii a5, 240
  diwb a13, 96
  dpfro a5, 20
  entry a5, 256
  extui a6, a7, 17, 4
  float.s f5, a8, 7
  ill.n
  loop a5, there
  l8ui a4, a11, 9
there:
  l16si a9, a12, 230
  l16ui a13, a10, 110
  l32ai a7, a15, 240
  l32e a2, a12, -64
  l32e a9, a13, -8
  l32i.n a3, a6, 32
  l32r a5, there
  ldct a5, a7
  lddec m3, a13
  lsi f5, a10, 240

