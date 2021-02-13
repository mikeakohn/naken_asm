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
  ;; Setup interrupt.
  mvii #vblank_interrupt, r0
  mvo r0, 0x100
  swap r0
  mvo r0, 0x101
  ;; Enable interrupts.
  eis

while_1:
  ;; Set character on the screen to white 'A'.
  mvii #(0x21 << 3) | 7, r0
  mvii #0x200, r1
  mvo@ r0, r1

  jsr r4, wait_for_1_second

  ;; Set character on the screen to blue 'Z'.
  mvii #(0x3a << 3) | 1, r0
  mvii #0x200, r1
  mvo@ r0, r1

  jsr r4, wait_for_1_second

  b while_1

wait_for_1_second:
  pshr r4
  ;; Count 60 interrupts to delay 1 second.
  mvii #60, r1
wait_for_1_second_loop:
  ;; Set did_vblank to 0.
  mvii #0, r0
  mvo r0, 0x0102
  ;; Wait for did_vblank to change to 1.
wait_on_interrupt:
  mvi 0x102, r0
  cmpi #1, r0
  bneq wait_on_interrupt
  decr r1
  bneq wait_for_1_second_loop
  pulr r7

vblank_interrupt:
  ;; Enable display.
  mvo r0, 0x0020
  ;; Set did_vblank to 1.
  mvii #1, r0
  mvo r0, 0x0102
  ;; Return from interrupt.
  jr r5

