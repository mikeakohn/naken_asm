{{ Propeller Mandelbrots         }}
{{ Copyright 2017 - Michael Kohn }}
{{ http://www.mikekohn.net/      }}
{{                               }}
{{ P0 - DOUT                     }}
{{ P1 - CLK                      }}
{{ P2 - CS                       }}
{{ P3 - D/C                      }}
{{ P4 - RES                      }}

CON
  _clkfreq = 5_000_000
  _clkmode = xtal1 + pll16x

  dout = 0
  clk = 1
  cs = 2
  dc = 3
  reset = 4
  led0 = 26
  led1 = 27

  command_display_off = $ae
  command_set_remap = $a0
  command_start_line = $a1
  command_display_offset = $a2
  command_normal_display = $a4
  command_set_multiplex = $a8
  command_set_master = $ad
  command_power_mode = $b0
  command_precharge = $b1
  command_clockdiv = $b3
  command_precharge_a = $8a
  command_precharge_b = $8b
  command_precharge_c = $8c
  command_precharge_level = $bb
  command_vcomh = $be
  command_master_current = $87
  command_contrasta = $81
  command_contrastb = $82
  command_contrastc = $83
  command_display_on = $af

'OBJ
'  Mandelbrot: "mandelbrot_code"

VAR
  ' 96 * 64 bytes of image, 6 words for signalling where
  ' word 0: start_signal=1
  ' word 1: 16 bit fixed point real
  ' word 2: 16 bit fixed point imaginary
  ' word 3: dx
  ' word 4: dy
  ' word 5: Row number
  ' word 6: reserved
  ' word 7: reserved
  byte image[(8 * 16) + (96 * 64)]
  byte lcd_cog
  byte row

Pub Main | index, ptr, r0, r1, i0, i1, dr0, dr1, di0, di1
  dira := $1f
  dira[led1] := 1
  dira[led0] := 1

  outa := $f
  outa[led0] := 0

  ' Wait a little bit with RES held low, then let RES go high
  WaitCnt(10_000_000 + Cnt)

  outa[reset] := 1
  outa[led0] := 0

  SendCommand(command_display_off)
  SendCommand(command_set_remap)
  SendCommand($72)
  SendCommand(command_start_line)
  SendCommand($00)
  SendCommand(command_display_offset)
  SendCommand($00)
  SendCommand(command_normal_display)
  SendCommand(command_set_multiplex)
  SendCommand($3f)
  SendCommand(command_set_master)
  SendCommand($8e)
  SendCommand(command_power_mode)
  SendCommand(command_precharge)
  SendCommand($31)
  SendCommand(command_clockdiv)
  SendCommand($f0)
  SendCommand(command_precharge_a)
  SendCommand($64)
  SendCommand(command_precharge_b)
  SendCommand($78)
  SendCommand(command_precharge_c)
  SendCommand($64)
  SendCommand(command_precharge_level)
  SendCommand($3a)
  SendCommand(command_vcomh)
  SendCommand($3e)
  SendCommand(command_master_current)
  SendCommand($06)
  SendCommand(command_contrasta)
  SendCommand($91)
  SendCommand(command_contrastb)
  SendCommand($50)
  SendCommand(command_contrastc)
  SendCommand($7d)
  SendCommand(command_display_on)

  ' Clear screen
  'repeat 96*64
  '  sendData($1)
  '  sendData(0)

  bytefill(@image, 0, (8 * 16) + (96 * 64))

  outa[dout] := 0
  outa[clk] := 0
  outa[cs] := 0
  outa[dc] := 0

  lcd_cog := 1

  coginit(lcd_cog, @lcd_code, @image)
  coginit(2, @mandelbrot, @image)
  coginit(3, @mandelbrot, @image)
  coginit(4, @mandelbrot, @image)
  coginit(5, @mandelbrot, @image)
  coginit(6, @mandelbrot, @image)
  coginit(7, @mandelbrot, @image)

  ' Signal LCD cog it's time to write and wait till it's finished
  image[lcd_cog * 16] := 1
  repeat while image[lcd_cog * 16] <> 0

  repeat
    repeat 4
      ComputeMandelbrot((-2 << 10), (-1 << 10), (1 << 10), (1 << 10))
      ComputeMandelbrot((-1 << 10), (-1 << 9), (1 << 9), (1 << 9))
      ComputeMandelbrot((-2 << 9), (-1 << 9), (0 << 10), (0 << 10))

    r0 := (-2 << 10)
    r1 := (1 << 10)
    i0 := (-1 << 10)
    i1 := (1 << 10)

    dr0 := ((-2 << 9) - r0) / 64
    dr1 := ((0 << 10) - r1) / 64
    di0 := ((-1 << 9) - i0) / 64
    di1 := ((0 << 10) - i1) / 64

    repeat 64
      ComputeMandelbrot(r0, i0, r1, i1)

      r0 := r0 + dr0
      r1 := r1 + dr1
      i0 := i0 + di0
      i1 := i1 + di1

Pub ComputeMandelbrot(r0, i0, r1, i1) | dx, dy, spinning, n, index, ptr

  dx := (r1 - r0) / 96
  dy := (i1 - i0) / 64
  row := 0

  repeat while row < 63
    ' Find free cog
    repeat index from 2 to 7
      ptr := index * 16

      if image[ptr] == 0
        image[ptr+2] := r0 & 255
        image[ptr+3] := r0 >> 8
        image[ptr+4] := i0 & 255
        image[ptr+5] := i0 >> 8
        image[ptr+6] := dx & 255
        image[ptr+7] := dx >> 8
        image[ptr+8] := dy & 255
        image[ptr+9] := dy >> 8
        image[ptr+10] := row & 255
        image[ptr+11] := row >> 8
        image[ptr] := 1

        i0 := i0 + dy
        row := row + 1

  spinning := 1

  ' Wait for all cores to finish
  repeat until spinning == 0
    spinning := 0

    repeat n from 2 to 7
      ptr := n * 16

      if image[ptr] == 1
        spinning := spinning + 1

  ' Signal LCD cog it's time to write and wait till it's finished
  image[lcd_cog * 16] := 1
  repeat while image[lcd_cog * 16] <> 0

Pub SendCommand(command)
  outa[led0] := 1
  outa[dc] := 0
  outa[cs] := 0

  repeat 8
    if (command & $80) <> 0
      outa[dout] := 1
    else
      outa[dout] := 0

    command := command << 1

    outa[clk] := 0
    outa[clk] := 1

  outa[cs] := 1
  outa[led0] := 0

Pub SendData(data)
  outa[led0] := 1
  outa[dc] := 1
  outa[cs] := 0

  repeat 8
    if (data & $80) <> 0
      outa[dout] := 1
    else
      outa[dout] := 0

    data := data << 1

    outa[clk] := 0
    outa[clk] := 1

  outa[cs] := 1
  outa[led0] := 0

DAT
  lcd_code file "lcd_code.bin"
  mandelbrot file "mandelbrot_code6.bin"



