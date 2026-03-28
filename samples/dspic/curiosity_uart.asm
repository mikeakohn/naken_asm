;; Here's a sample of UART for the Microchip dsPIC33CK Curiosity Nano board.

.dspic
.include "p33ck64mc105.inc"

UARTEN equ 15
UTXEN  equ 5
URXEN  equ 4

.org 0
  goto start

.org 0x200

start:
  clr w0
  mov.w wreg, U1MODEH
  mov.w wreg, U1STA
  mov.w wreg, U1STAH

  mov #25, w0
  mov.w wreg, U1BRG

  mov #(1 << UARTEN) | (1 << UTXEN) | (1 << URXEN), w0
  mov.w wreg, U1MODE

  mov #1, w0
  mov.w wreg, RPOR13;
  mov #59, w0
  mov.w wreg, RPINR18;

  mov #'A', w0
  mov.w wreg, U1TXREG;
  mov #'B', w0
  mov.w wreg, U1TXREG;
  mov #'C', w0
  mov.w wreg, U1TXREG;

  bclr TRISD, #10

main:
  bclr LATD, #10
  call wait_uart_rx_available
  mov.w U1RXREG, wreg
  ;mov #'1', w0
  mov.w wreg, U1TXREG;

  bset LATD, #10
  call wait_uart_rx_available
  mov.w U1RXREG, wreg
  ;mov #'1', w0
  mov.w wreg, U1TXREG;

  bra main

wait_uart_rx_available:
  btst U1STAH, #1
  bra nz, wait_uart_rx_available
  return

delay:
  mov #0xffff, w4
repeat_loop:
  dec w4, w4
  bra nz, repeat_loop
  return


