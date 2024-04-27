.f8

main:
  ins 8
  in 16
  bno label_1
  bt 2, label_1
  lm
  lr a, 4
  lr 4, a
  lr isar, a
  lr a, ku
  lr ql, a
  nop
  pop
  dci label_2
  li 120
  lis 7
  amd
  oi 0x23
  lr w, j
  lr j, w
  jmp label_2
  asd 3
  asd isar
  lr dc0, q
  lr pc0, q
  lr pc1, k
  lr h, dc0

label_1:
  lr a, s
  lr a, s+
  lr a, s-
  lr isar, a
  lr isar+, a
  lr isar-, a

label_2:



