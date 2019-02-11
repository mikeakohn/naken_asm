.copper

main:
  wait 50, 51, 0x7f, 0x7f
  wait 50, 50
  wait bfd, 50, 50, 0x7f, 0x7f
  wait bfd, 50, 50
  wait

  move 0xe2, 0x1000

  skip 50, 50, 0x7f, 0x7f
  skip 50, 50
  skip bfd, 50, 50, 0x7f, 0x7f
  skip bfd, 50, 50

