const char *msp430_1 =
  ".msp430\n"
  ".org 0x200\n"
  "first:\n"
  "  mov.w #1, &100\n"
  "second:\n"
  "  mov.w #first, &100\n"
  "third:\n"
  "  mov.w #fifth, &100\n"
  "fourth:\n"
  "  mov.b #1, &100\n"
  ".org 2\n"
  "fifth:\n"
  "  mov.w #fourth, &100\n\n";

const char *msp430_2 =
  ".msp430x\n"
  ".org 0x200\n"
  "first:\n"
  "  mov.w #1, &100\n"
  "second:\n"
  "  mova &first, r7\n"
  "third:\n"
  "  mova r7, &100\n"
  "fourth:\n"
  "  mov.b #1, &100\n"
  ".org 2\n"
  "fifth:\n"
  "  mov.w #fourth, &100\n\n";

const char *msp430_3 =
  ".msp430x\n"
  ".org 0x200\n"
  "first:\n"
  "  rpt #5, adda #7, r8\n"
  "second:\n"
  "  calla fifth\n"
  "third:\n"
  "  mova r7, &100\n"
  "fourth:\n"
  "  mov.b #1, &100\n"
  ".org 2\n"
  "fifth:\n"
  "  mov.w #fourth, &100\n\n";


