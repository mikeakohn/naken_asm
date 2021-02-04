.cp1610

.define BIDECLE(a) .dw a & 0xff, a >> 8

.org 0x57ff
  .dw 0x0000

.org 0x5000

rom_header:
  BIDECLE(zero)       ; MOB picture base   (points to NULL list)
  BIDECLE(zero)       ; Process table      (points to NULL list)
  BIDECLE(main)       ; Program start address
  BIDECLE(zero)       ; Bkgnd picture base (points to NULL list)
  BIDECLE(ones)       ; GRAM pictures      (points to NULL list)
  BIDECLE(title)      ; Cartridge title/date
  .dw 0x03c0          ; Flags:  No ECS title, run code after title,
zero:
  .dw 0x0000          ; Screen border control
  .dw 0x0000          ; 0 = color stack, 1 = f/b mode
  BIDECLE(ones)       ; GRAM pictures      (points to NULL list)
ones:
  .dw 1, 1, 1, 1, 1   ; Color stack initialization

title:
  .dw 107, 'H', 'e', 'l', 'l', 'o', 0

main:
  eis
while_1:
  b while_1

