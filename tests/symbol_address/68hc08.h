const char *m68hc08_1 =
  ".68hc08\n"
  ".org 0x1000\n"
  "first:\n"
  "  lda 10, SP\n"
  "second:\n"
  "  lda first, SP\n"
  "third:\n"
  "  lda 0x1000\n"
  "  lda fifth\n"
  "fourth:\n"
  ".org 0xf0\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

const char *m68hc08_2 =
  ".68hc08\n"
  ".org 0x1000\n"
  "first:\n"
  "  sub 10\n"
  "second:\n"
  "  sub first\n"
  "third:\n"
  "  sub 0x1000\n"
  "  sub fifth\n"
  "fourth:\n"
  ".org 0xf0\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

const char *m68hc08_3 =
  ".68hc08\n"
  ".org 0x1000\n"
  "first:\n"
  "  ldx 10, X\n"
  "second:\n"
  "  ldx first, X\n"
  "third:\n"
  "  ldx 0x1000, X\n"
  "  ldx fifth, X\n"
  "fourth:\n"
  ".org 0xf0\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

