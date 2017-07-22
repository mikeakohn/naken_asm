; test program that clears screen and plots an asterisk on a c64
; by Joe Davisson
; simulator test:
; set pc=0x816, speed=999999, run
; bprint 0x400-0x500, an asterisk (0x2a) should appear at location 0x4cd

.6502

// zp locations required for indirection
.define resultlo 0xfb
.define resulthi 0xfc

; basic loader
.org 0x0801 - 2
; basic load address
dw 0x0801
dw start
; line number
dw 2013
; sys 2070
db 0x9e
.ascii "2070"
db 0x3a, 0x8f, 0x20, 0x4d, 0x4f, 0x52, 0x54, 0x49, 0x53
db 0
dw 0

; program start
start:
  cld

; clear screen
  jsr clrscr
  lda #0
  sta 53281
  lda #11
  sta 53280

; plot a white asterisk at 5,5
  lda #5
  sta plotx
  lda #5
  sta ploty
  lda #$2a
  sta plotchar
  lda #1
  sta plotcolor
  jsr plot

; exit (use brk for simulator)
  ;brk
  rts

clrscr:
  lda #0x20
  ldx #0
clrloop:
  sta 0x400,x
  sta 0x500,x
  sta 0x600,x
  sta 0x700,x
  inx
  bne clrloop
  rts

plot:
  lda #0x00
  sta resultlo
  lda #0x04
  sta resulthi

  ldy ploty
plotloopy:
  clc
  lda resultlo
  adc #40
  sta resultlo
  bcc plotskipy
  inc resulthi
plotskipy:
  dey
  bne plotloopy

  clc
  lda resultlo
  adc plotx
  sta resultlo
  bcc plotskipx
  inc resulthi

plotskipx:
  lda plotchar
  ldy #0
  sta (resultlo),y

  lda resultlo
  sta num1lo
  lda resulthi
  sta num1hi

  lda #((0xD800 - 0x400) & 0xFF)
  sta num2lo
  lda #((0xD800 - 0x400) >> 8)
  sta num2hi

  jsr add16

  lda plotcolor
  ldy #0
  sta (resultlo),y
  rts

add16:
  clc
  lda num1lo
  adc num2lo
  sta resultlo
  lda num1hi
  adc num2hi
  sta resulthi
  rts

sub16:
  sec
  lda num1lo
  sbc num2lo
  sta resultlo
  lda num1hi
  sbc num2hi
  sta resulthi
  rts
  
plotx:
  db 0
ploty:
  db 0
plotchar:
  db 0
plotcolor:
  db 0

num1lo:
  db 0 
num1hi:
  db 0 
num2lo:
  db 0 
num2hi:
  db 0 

