const char *msp430 =
  ".msp430\n"
  ".org 0x200\n"
  "first:\n"
  "  mov.w #1, &100\n"
  "second:"
  "  mov.w #first, &100\n"
  "third:"
  "  mov.w #fifth, &100\n"
  "fourth:"
  "  mov.b #1, &100\n"
  "fifth:"
  "  mov.w #fourth, &100\n\n";

