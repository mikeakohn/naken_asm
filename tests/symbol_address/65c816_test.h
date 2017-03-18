const char *w65c816 =
  ".65816\n"
  ".org 0x1000\n"
  "first:\n"
  "  lda #10\n"
  "second:\n"
  "  lda.b #10\n"
  "third:\n"
  "  ldx fifth\n"
  "fourth:\n"
  ".org 0xf0\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

