.epiphany

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

.org 0x2000
_sync_interrupt:

main:
  ;; v1 will hold the data to write to local memory
  ;; v2 will point to local memory
  ;; v3 will point to the first location in external memory (0x8e000000)
  mov v1, #10
  mov v2, #data
  mov v3, #0x0000
  movt v3, #0x8e00

  ;; Store the local address pointer in local memory indexed by 1 word
  ;; Store the local address pointer in external memory
  str v2, [v2,#1]
  str v2, [v3]

loop:
  ;; Increment counter and store in local memory
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
  dc32 0
address:
  dc32 0

