.epiphany

.org 0
.org 0x00
  b _sync_interrupt
.org 0x04
  b _sw_exception_interrupt
.org 0x08
  b _memory_fault_interrupt
.org 0x0c
  b _timer0_interrupt
.org 0x10
  b _timer1_interrupt
.org 0x14
  b _message_interrupt
.org 0x18
  b _dma0_interrupt
.org 0x1c
  b _dma1_interrupt
.org 0x20
  b _wand_interrupt
.org 0x24
  b _user_interrupt


.org 0x40
_sync_interrupt:

main:
  mov v1, #0
  mov v2, #data
loop:
  add v1, v1, #1
  str v1, [v2]
  b loop

_sw_exception_interrupt:
_memory_fault_interrupt:
_timer0_interrupt:
_timer1_interrupt:
_message_interrupt:
_dma0_interrupt:
_dma1_interrupt:
_wand_interrupt:
_user_interrupt:
  rti

.org 0x6000
data:

