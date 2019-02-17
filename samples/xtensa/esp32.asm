;; Blink pin 27 of SparkFun's ESP32 Thing
;; https://www.sparkfun.com/products/13907

;; This doesn't work yet.

.xtensa
.include "esp32.inc"
;.high_address 0x4000_0000 + 2097151

.org 0x4000_0000
start:
  j main

constants:
gpio_base:
  dc32 GPIO_BASE
const_1_27:
  dc32 1 << 27
gpio_base_1:
  dc32 GPIO_BASE + 0x500

main:
  ;; Load a10 as a pointer to GPIO registers.
  ;nop.n
  nop
  nop
  nop
  l32r a9, constants
  l32i a10, a9, 0
  l32i a12, a9, 8

  ;; Set GPIO 27's MUX to GPIO.
  ;movi a11, 1
  ;s32i a11, a10, GPIO_MUX_GPIO27_REG

  ;; Set GPIO 5 to enabled and to a 1.
  ;l32i a11, a9, 4
  movi a11, 1 << 5
  s32i a11, a10, GPIO_ENABLE_REG
  movi a11, 0
  s32i a11, a10, IO_MUX_GPIO5_REG
  movi a11, 0
  ;s32i a11, a10, GPIO_FUNC5_OUT_SEL_CFG_REG
  s32i a11, a12, 0x44
  ;movi a11, 1 << 5
  movi a11, 0
  s32i a11, a10, GPIO_OUT_REG

while_1:
  j while_1

