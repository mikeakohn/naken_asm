const char *w65c816_1 =
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

const char *w65c816_2 =
  ".65816\n"
  ".org 0xff00\n"
  "first:\n"
  "  lda fifth, x\n"
  "second:\n"
  "  lda first, x\n"
  "third:\n"
  "  bne first\n"
  "fourth:\n"
  ".org 0xfffc\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

