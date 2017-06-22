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

Pub Main | count
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
  repeat 96*64
    sendData($1)
    sendData(0)

  'repeat 96*5
  '  sendData($0)
  '  sendData($f)

  cognew(@code, 0)

  outa[dout] := 0
  outa[clk] := 0
  outa[cs] := 0
  outa[dc] := 0

  repeat
    count := count + 1
    'outa[led0] := 1
    'outa[led1] := 1

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
  code file "mandelbrot_code.bin"



