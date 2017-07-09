const char *w6502_1 =
  ".6502\n"
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

const char *w6502_2 =
  ".6502\n"
  ".org 0x1000\n"
  "first:\n"
  "  trb fifth\n"
  "second:\n"
  "  trb first\n"
  "third:\n"
  "  bbs4 $10, first\n"
  "fourth:\n"
  ".org 0xf0\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

