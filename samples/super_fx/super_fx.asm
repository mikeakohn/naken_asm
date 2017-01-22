.super_fx

main:
  jmp r9
  inc r5
  getc
  link #1
  lms r5, (0x40)
  ibt r5, #0xf9
  iwt r5, #0xeef9
  nop
  ;lea r5, 23
  ;move r5, r6
  ;move r5, #5
  ;move r5, (0x40)
  ;move (0x40), r5
  ;moveb r5, (r6)
  ;moveb (r6), r5
  ;moves r5, r6
  ;movew r6, (r5)
  ;movew (r5), r6

