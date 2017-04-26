const char *stm8 =
  ".stm8\n"
  ".org 0x1000\n"
  "first:\n"
  "  ld a, 0x10\n"
  "second:\n"
  "  ld a, fifth\n"
  "third:\n"
  "  ld a, (fifth,Y)\n"
  "fourth:\n"
  ".org 0xf0\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

