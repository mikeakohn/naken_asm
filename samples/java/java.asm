.java

start:
  iadd
  iand
  getstatic 5
  lload 7
  lload 256
  lload test_1
  lload test_2

  newarray boolean
  newarray 3

  iinc 5, 3

  if_icmpeq test_2
  goto_w start

test_1:
.org 1000
test_2:

