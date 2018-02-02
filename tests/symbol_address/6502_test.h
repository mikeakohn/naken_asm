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

const char *w6502_3 =
  ".6502\n"
  ".org 0xff00\n"
  "first:\n"
  "  lda fifth, x\n"
  "second:\n"
  "  lda first, x\n"
  "third:\n"
  "  lda fifth, y\n"
  "fourth:\n"
  ".org 0xfffc\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

const char *w6502_4 =
  ".6502\n"
  ".org 0xff00\n"
  "first:\n"
  "  lda fifth\n"
  "second:\n"
  "  lda first\n"
  "third:\n"
  "  bne first\n"
  "fourth:\n"
  ".org 0xfffc\n"
  "fifth:\n"
  "  db 0x10, 0x20\n";

