const char *mc68000 =
  ".68000\n"
  ".org 0x1000\n"
  "first:\n"
  "  move.l #fifth, d0\n"
  "second:\n"
  "  add.l (fifth), d1\n"
  "third:\n"
  "  sub.w #1, d5\n"
  "fourth:\n"
  ".org 0xf0\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

