;; Waveshare 0.95 Inch RGB OLED.
;;
;; Copyright 2020 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Example of using a Parallax Propeller 2 with a Waveshare 96x64 OLED
;; display (sold by Parallax) interfaced with an SPI SSD1331 chip.

.propeller2

lcd_dout  equ 8
lcd_clk   equ 10
lcd_cs    equ 12
lcd_dc    equ 14
lcd_reset equ 15
led_0     equ 56
led_1     equ 57

start:
  outh #lcd_reset
  outh #lcd_cs
  mov dira, porta_dir
  ;mov dirb, ##(1 << led_0) | (1 << led_1)
  ;outl #56
  ;outl #57

  ;; Signal is start of buffer + (cogid * 16).
  mov signal, ptra
  cogid temp
  shl temp, #4
  add signal, temp

  ;; Image is start of buffer + (signal buffer).
  mov image, ptra
  add image, #8 * 16

  ;; Wait for spin code to signal this cog to read the image.
main:
  ;rdbyte temp, signal, wz
  ;if_z jmp #main

  waitatn

  ;; Point to image and set a counter for 96 * 64 pixels.
  mov count, const_6144
  mov image_ptr, image

next_pixel:
  ;; pixel = colors[image_ptr++]
  rdbyte temp, image_ptr
  add image_ptr, #1
  add temp, #colors
  ;; Alter the S value (the 0) of mov pixel, 0 to the value in temp.
  alts temp
  mov pixel, 0

  ;mov pixel, ##0xf800

  ;; Write pixel to LCD
  mov data, pixel
  shr data, #8
  and data, #0xff
  call #send_data
  mov data, pixel
  and data, #0xff
  call #send_data

  ;; count--
  djnz count, #next_pixel

  ;; Reset the the signal so the SPIN cog knows the LCD is done
  wrbyte zero, signal

  jmp #main

;; send_data(data)
send_data:
  ;; Set DC = 1 (for data) and CS = 0.
  outh #lcd_dc
  outl #lcd_cs

  rol data, #24
  mov bit_count, #8

send_data_next_bit:
  ;; data := data << 1
  ;; If carry bit is set, dout = 1, otherwise 0.
  rol data, #1, wc

  if_c outh #lcd_dout
  if_nc outl #lcd_dout

  ;; Set CLK to 1 then 0 to clock out bit.
  outh #lcd_clk
  outl #lcd_clk

  djnz bit_count, #send_data_next_bit

  ;; Set CS = 1 to end data transmission.
  outh #lcd_cs
  ret

bit_count:
  dc32 0
data:
  dc32 0x0
porta_dir:
  dc32 (1 << lcd_dout) | (1 << lcd_clk) | (1 << lcd_cs) | (1 << lcd_dc) | (1 << lcd_reset)
temp:
  dc32 0
signal:
  dc32 0
image:
  dc32 0
const_6144:
  dc32 6144
count:
  dc32 0
image_ptr:
  dc32 0
pixel:
  dc32 0
zero:
  dc32 0
colors:
  dc32 0x0000
  dc32 0x000c
  dc32 0x0013
  dc32 0x0015
  dc32 0x0195
  dc32 0x0335
  dc32 0x04d5
  dc32 0x34c0
  dc32 0x64c0
  dc32 0x9cc0
  dc32 0x6320
  dc32 0xa980
  dc32 0xaaa0
  dc32 0xcaa0
  dc32 0xe980
  dc32 0xf800

