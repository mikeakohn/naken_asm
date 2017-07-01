const char *epiphany =
  ".epiphany\n"
  ".org 0x200\n"
  "first:\n"
  "  add r1, r2, #fifth\n"
  "second:\n"
  "  b fourth\n"
  "third:\n"
  "  lsr r1, r2, r3\n"
  "fourth:\n"
  "  mov r3, #fifth\n"
  ".org 2\n"
  "fifth:\n"
  "  add r2, r3, #fourth\n\n";

