;; DS18B20 temperature sensor test
;;
;; Copyright 2011-2017 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike to mikekohn.net
;;

.include "msp430x2xx.inc"

RAM equ 0x0200
DS18B20_DATA equ 0x0210
DS18B20_CMD equ 0x0220
DS18B20_CMD_LEN equ 0x0221
DS18B20_BIT equ 0x0222
DEBUG_STATE equ 0x0223

;  r4 =
;  r5 =
;  r6 =
;  r7 =
;  r8 =
;  r9 =
; r10 =
; r11 =
; r12 =
; r13 =
; r14 =
; r15 =

.org 0xf800
start:
  ;; Turn off watchdog
  mov.w #(WDTPW|WDTHOLD), &WDTCTL

  ;; Interrupts off
  dint

  ;; r13 points to which interrupt routine should be called
  mov.w #null_interrupt, r13

  ;; Set up stack pointer
  mov.w #0x0280, SP

  ;; Set MCLK to 16 MHz with DCO 
  mov.b #(DCO_7), &DCOCTL
  mov.b #RSEL_14, &BCSCTL1
  mov.b #0, &BCSCTL2

.if 0
  ;; Set MCLK to 16 MHz external crystal
  bic.w #OSCOFF, SR
  bis.b #XTS, &BCSCTL1
  mov.b #LFXT1S_3, &BCSCTL3
  ;mov.b #LFXT1S_3|XCAP_1, &BCSCTL3
test_osc:
  bic.b #OFIFG, &IFG1
  mov.w #0x00ff, r15
dec_again:
  dec r15
  jnz dec_again
  bit.b #(OFIFG), &IFG1
  jnz test_osc
  mov.b #(SELM_3|SELS), &BCSCTL2
.endif

  ;; Set up output pins
  ;; P1.0 = Data Out for DS18B20
  ;; P1.1 =  CE for RTC
  ;; P1.2 = /CS for DAC
  ;; P1.3 = /CS for SD
  mov.b #0x0f, &P1DIR        ; P1.0, P1.1, P1.2, P1.3
  mov.b #(8|4|1), &P1OUT

  ;; Set up Timer
  mov.w #2000, &TACCR0
  mov.w #(TASSEL_2|MC_1), &TACTL ; SMCLK, DIV1, COUNT to TACCR0
  mov.w #CCIE, &TACCTL0
  mov.w #0, &TACCTL1

  ;; Interrupts back on
  eint

  call #read_temp

main:
  jmp main

read_temp:
  mov.w #ds18b20_signal_interrupt, r13
  ;; First do a convert
  call #init_ds18b20
  mov.b #0xcc, &DS18B20_CMD
  call #send_ds18b20_cmd
  mov.b #0x44, &DS18B20_CMD
  call #send_ds18b20_cmd
  ;;mov.w #0, r4  ;; DEBUG
  call #wait_ds18b20_convert

  ;; Second fetch the scratch-pad
  call #init_ds18b20
  mov.b #0xcc, &DS18B20_CMD
  call #send_ds18b20_cmd
  mov.b #0xbe, &DS18B20_CMD
  call #send_ds18b20_cmd
  call #fetch_ds18b20_scratch_pad

  mov.w #null_interrupt, r13
  ret

;; init DS18B20
init_ds18b20:
  ;mov.w #0, r4
  ;mov.w #0, r5
  bic.b #1, P1OUT
  mov.w #7680, &TACCR0   ; 480us
  mov.w #0, TAR
  mov.w #0, r11
master_reset_pulse:
  cmp.w #1, r11
  jne master_reset_pulse
  bis.b #1, P1OUT
  bic.b #1, P1DIR
wait_slave_low:
  ;inc r4
  bit.b #1, P1IN
  jnz wait_slave_low
wait_slave_hi:
  ;inc r5
  bit.b #1, P1IN
  jz wait_slave_hi
  bis.b #1, P1DIR
  mov.w #960, &TACCR0   ; 60us
  ret

;; send_ds18b20_cmd
send_ds18b20_cmd:
  mov.b #8, &DS18B20_CMD_LEN
send_next_bit:
  bit.b #1, &DS18B20_CMD
  jz cmd_bit_0
  call #write_ds18b20_1
  jmp cmd_bit_done
cmd_bit_0:
  call #write_ds18b20_0
cmd_bit_done:
  mov.w #6, r10
cmd_wait_1us:
  dec r10
  jnz cmd_wait_1us
  rra.b &DS18B20_CMD
  dec.b &DS18B20_CMD_LEN
  jnz send_next_bit
  ret

;; wait_ds18b20_convert
wait_ds18b20_convert:
  mov.w #6, r10
convert_wait_1us:
  dec r10
  jnz convert_wait_1us
  call #read_ds18b20
  cmp.w #0, r10
  jeq wait_ds18b20_convert
  ret

;; fetch_ds18b20_scratch_pad
fetch_ds18b20_scratch_pad:
  mov.w #DS18B20_DATA, r14
fetch_next_byte:
  mov.w #8, &DS18B20_BIT

fetch_next_bit:
  mov.w #6, r10
scratch_wait_1us:
  dec r10
  jnz scratch_wait_1us

  call #read_ds18b20
  rra.b 0(r14)
  cmp.w #0, r10
  jeq fetched_0
  bis.b #128, 0(r14)
  jmp done_set_fetch
fetched_0:
  bic.b #128, 0(r14)
done_set_fetch:
  dec &DS18B20_BIT
  jnz fetch_next_bit

  inc r14
  cmp.w #DS18B20_DATA+9, r14
  jne fetch_next_byte
  ret

;; write 1 to DS18B20
write_ds18b20_1:
  bic.b #1, P1OUT
  mov.w #0, TAR
  mov.w #0, r11
  mov.w #6, r10
wait_at_least_1us_write_1:
  dec r10
  jnz wait_at_least_1us_write_1
  bis.b #1, P1OUT
master_write_1:
  cmp.w #1, r11
  jne master_write_1
  ret

;; write 0 to DS18B20
write_ds18b20_0:
  bic.b #1, P1OUT
  mov.w #0, TAR
  mov.w #0, r11
master_write_0:
  cmp.w #1, r11
  jne master_write_0
  bis.b #1, P1OUT
  ret

;; read from DS18B20 (returns 0 in r10 for a 0, return 1 for a 1)
read_ds18b20:
  bic.b #1, P1OUT
  mov.w #6, r10
wait_at_least_1us_read:
  dec r10
  jnz wait_at_least_1us_read
  bis.b #1, P1OUT
  bic.b #1, P1DIR
  mov.w #0, TAR
  mov.w #0, r11
wait_read_bit:
  bit.b #1, P1IN
  jnz read_bit_not_zero
  inc r10
read_bit_not_zero:
  cmp.w #1, r11
  jne wait_read_bit
  bis.b #1, P1DIR

  cmp.w #0, r10
  jeq return_1
  mov.w #0, r10
  ret
return_1:
  mov.w #1, r10
  ret

timer_interrupt:
  br r13

null_interrupt:
  reti

ds18b20_signal_interrupt:
  mov.w #1, r11
  reti

.org 0xffe8
vectors:
  dw 0
  dw 0
  dw 0
  dw 0
  dw 0
  dw timer_interrupt       ; Timer_A2 TACCR0, CCIFG
  dw 0
  dw 0
  dw 0
  dw 0
  dw 0
  dw start                 ; Reset



