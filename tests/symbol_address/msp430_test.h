const char *msp430 =
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

