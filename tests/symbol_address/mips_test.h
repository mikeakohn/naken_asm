const char *mips =
  ".mips\n"
  ".org 0x1000\n"
  "first:\n"
  "  li $v0, fifth\n"
  "second:\n"
  "  li $v1, 0xf1220000\n"
  "third:\n"
  "  li $a0, first\n"
  "fourth:\n"
  ".org 0xf1220000\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

