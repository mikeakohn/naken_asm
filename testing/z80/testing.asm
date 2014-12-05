.z80

main:
  adc  a,a           ; 8F
  add  a,a           ; 87
  ld   a,a           ; 7F
  sbc  a,a           ; 9F
  adc  a,b           ; 88
  add  a,b           ; 80
  ld   a,b           ; 78
  sbc  a,b           ; 98
  adc  a,c           ; 89
  add  a,c           ; 81
  ld   a,c           ; 79
  sbc  a,c           ; 99
  adc  a,d           ; 8A
  add  a,d           ; 82
  ld   a,d           ; 7A
  sbc  a,d           ; 9A
  adc  a,e           ; 8B
  add  a,e           ; 83
  ld   a,e           ; 7B
  sbc  a,e           ; 9B
  adc  a,h           ; 8C
  add  a,h           ; 84
  ld   a,h           ; 7C
  sbc  a,h           ; 9C

  and  a             ; A7
  and  b             ; A0
  and  c             ; A1
  and  d             ; A2
  and  e             ; A3
  and  h             ; A4
  and  l             ; A5
  and  (hl)          ; A6

  and  ixh           ; DD A4
  and  ixl           ; DD A5
  and  iyh           ; FD A4
  and  iyl           ; FD A5

  and  7             ; E6 N

  and  (ix+5)          ; DD A6 00
  and  (iy)          ; FD A6 00



  add  ix,bc         ; DD 09
  add  ix,de         ; DD 19
  add  ix,ix         ; DD 29
  add  ix,sp         ; DD 39
  add  iy,bc         ; FD 09
  add  iy,de         ; FD 19
  add  iy,iy         ; FD 29
  add  iy,sp         ; FD 39

  sbc  a,ixh         ; DD 9C
  sbc  a,ixl         ; DD 9D
  sbc  a,iyh         ; FD 9C
  sbc  a,iyl         ; FD 9D

  add  a,(ix+5)      ; DD 86 DIS
  add  a,(ix-1)      ; DD 86 NDIS
  add  a,(iy)        ; FD 86 00
  add  a,(iy+10)     ; FD 86 DIS
  add  a,(iy-10)     ; FD 86 NDIS

  sbc  a,5           ; DE N
  sbc  hl,bc         ; ED 42
  sbc  hl,de         ; ED 52
  sbc  hl,hl         ; ED 62
  sbc  hl,sp         ; ED 72
  add  hl,bc         ; 09
  add  hl,de         ; 19
  add  hl,hl         ; 29
  add  hl,sp         ; 39
  adc  hl,bc         ; ED 4A
  adc  hl,de         ; ED 5A
  adc  hl,hl         ; ED 6A
  adc  hl,sp         ; ED 7A

  ccf                ; 3F
  reti               ; ED 4D
  ret                ; C9
  ;stop               ; DD DD 00

  adc  a,(hl)        ; 8E

  sub  (hl)          ; 96
  sub  5             ; D6 N

  ind                ; ED AA
  indr               ; ED BA

  call 0x1000        ; CD NNl NNh
  call c,0x1000      ; DC NNl NNh
  call m,0x1000      ; FC NNl NNh
  call nc,0x1000     ; D4 NNl NNh
  call nz,0x1000     ; C4 NNl NNh
  call p,0x1000      ; F4 NNl NNh
  call pe,0x1000     ; EC NNl NNh
  call po,0x1000     ; E4 NNl NNh
  call z,0x1000      ; CC NNl NNh

  dec  (hl)          ; 35
  dec  (ix)          ; DD 35 00
  dec  (ix+5)      ; DD 35 DIS
  dec  (ix-5)     ; DD 35 NDIS
  dec  (iy)          ; FD 35 00
  dec  (iy+7)      ; FD 35 DIS
  dec  (iy-7)     ; FD 35 NDIS

  dec  a             ; 3D
  dec  b             ; 05
  dec  c             ; 0D
  dec  d             ; 15
  dec  e             ; 1D
  dec  h             ; 25
  dec  l             ; 2D


  dec  bc            ; 0B
  dec  de            ; 1B
  dec  hl            ; 2B
  dec  sp            ; 3B

  dec  ix            ; DD 2B
  dec  iy            ; FD 2B

  dec  ixh           ; DD 25
  dec  ixl           ; DD 2D
  dec  iyh           ; FD 25
  dec  iyl           ; FD 2D

  xor  (hl)          ; AE
  xor  (ix)          ; DD AE 00
  xor  (ix+5)        ; DD AE DIS
  xor  (ix-5)        ; DD AE NDIS
  xor  (iy)          ; FD AE 00
  xor  (iy+5)        ; FD AE DIS
  xor  (iy-5)        ; FD AE NDIS
  xor  7             ; EE N
  xor  a             ; AF
  xor  b             ; A8
  xor  c             ; A9
  xor  d             ; AA
  xor  e             ; AB
  xor  h             ; AC
  xor  ixh           ; DD AC
  xor  ixl           ; DD AD
  xor  iyh           ; FD AC
  xor  iyl           ; FD AD
  xor  l             ; AD


  sub  (hl)          ; 96
  sub  (ix)          ; DD 96 00
  sub  (ix+8)        ; DD 96 DIS
  sub  (ix-8)        ; DD 96 NDIS
  sub  (iy)          ; FD 96 00
  sub  (iy+8)        ; FD 96 DIS
  sub  (iy-8)        ; FD 96 NDIS
  sub  9             ; D6 N
  sub  a             ; 97
  sub  b             ; 90
  sub  c             ; 91
  sub  d             ; 92
  sub  e             ; 93
  sub  h             ; 94
  ;sub  hl,bc         ; B7 ED 42
  ;sub  hl,de         ; B7 ED 52
  ;sub  hl,hl         ; B7 ED 62
  ;sub  hl,sp         ; B7 ED 72
  sub  ixh           ; DD 94
  sub  ixl           ; DD 95
  sub  iyh           ; FD 94
  sub  iyl           ; FD 95
  sub  l             ; 95

  ei                 ; FB
  ex   (sp),hl       ; E3
  ex   (sp),ix       ; DD E3
  ex   (sp),iy       ; FD E3
  ex   af,af'        ; 08
  ex   de,hl         ; EB
  exx                ; D9

  im   0             ; ED 46
  im   1             ; ED 56
  im   2             ; ED 5E

  in   a,(10)         ; DB N
  in   a,(c)         ; ED 78
  in   b,(c)         ; ED 40
  in   c,(c)         ; ED 48
  in   d,(c)         ; ED 50
  in   e,(c)         ; ED 58
  in   f,(c)         ; ED 70
  in   h,(c)         ; ED 60
  in   l,(c)         ; ED 68
  inc  (hl)          ; 34
  inc  (ix)          ; DD 34 00

  jp   (hl)          ; E9
  jp   (ix)          ; DD E9
  jp   (iy)          ; FD E9
  jp   10             ; C3 NNl NNh
  jp   c,10          ; DA NNl NNh
  jp   m,10          ; FA NNl NNh
  jp   nc,10         ; D2 NNl NNh
  jp   nz,10         ; C2 NNl NNh
  jp   p,10          ; F2 NNl NNh
  jp   pe,10         ; EA NNl NNh
  jp   po,10         ; E2 NNl NNh
  jp   z,10          ; CA NNl NNh
blah:
  jr   blah            ; 18 NNo
  jr   forward            ; 18 NNo
  jr   c,blah          ; 38 NNo
  ;jr   m,blah          ; FA NNl NNh
  jr   nc,blah         ; 30 NNo
  jr   nz,blah         ; 20 NNo
  jr   nz,forward         ; 20 NNo
  ;jr   p,10          ; F2 NNl NNh
  ;jr   pe,10         ; EA NNl NNh
  ;jr   po,10         ; E2 NNl NNh
  jr   z,blah          ; 28 NNo
  jr   z,forward          ; 28 NNo

  ld   l,a           ; 6F
  ld   l,b           ; 68
  ld   l,c           ; 69
  ld   l,d           ; 6A
forward:
  ld   l,e           ; 6B
  ld   l,h           ; 6C
  ld   l,l           ; 6D
  ld   a,a           ; 7F
  ld   a,b           ; 78
  ld   a,c           ; 79
  ld   a,d           ; 7A
  ld   a,e           ; 7B
  ld   a,h           ; 7C
  ld   a,l           ; 7D
  ld   b,a           ; 47
  ld   b,b           ; 40
  ld   b,c           ; 41
  ld   b,d           ; 42
  ld   b,e           ; 43
  ld   b,h           ; 44

  ;ld   a,ixh         ; DD 7C
  ;ld   a,ixl         ; DD 7D
  ;ld   c,iyh         ; FD 7C
  ;ld   a,iyl         ; FD 7D
  ;ld   b,ixh         ; DD 44
  ;ld   b,ixl         ; DD 45
  ;ld   d,iyh         ; FD 44
  ;ld   h,iyl         ; FD 45

  ;ld   ixl,a         ; DD 6F
  ;ld   ixh,b         ; DD 68
  ;ld   iyl,c         ; DD 69
  ;ld   iyh,d         ; DD 6A
  ;ld   ixl,e         ; DD 6B

  ;ld   ixl,ixh       ; DD 64
  ;ld   ixh,ixl       ; DD 65
  ;ld   ixl,ixh       ; DD 6C
  ;ld   ixh,ixl       ; DD 6D

  ld   c,10           ; 0E N
  ;ld   (hl),a        ; 77
  ;ld   (hl),b        ; 70

  ld   b,(hl)         ; 46
  ld   h,(hl)         ; 66

  ld   (hl),0x33      ; 36 N

  ld   a,(ix+12)      ; DD 7E DIS
  ld   b,(ix-11)      ; DD 7E N12
  ld   c,(iy)         ; FD 7E 00
  ld   h,(iy+12)      ; FD 7E DIS
  ld   (hl),(iy-11)   ; FD 7E N12

  ld   (ix+12),a     ; DD 77 DIS
  ld   (ix+12),b     ; DD 70 DIS
  ld   (iy-11),a     ; FD 77 N12
  ld   (iy-11),b     ; FD 70 N12
  ld   (ix+12),10    ; DD 36 DIS N

  ld   (hl),e        ; 73
  ld   (hl),h        ; 74
  ld   (hl),l        ; 75
  ld   (hl),10       ; 36 N

  ld   a,(bc)        ; 0A
  ld   a,(de)        ; 1A
  ld   (bc),a        ; 02
  ld   (de),a        ; 12

  ld   (10),a        ; 32 NNl NNh
  ld   a,(10)        ; 3A NNl NNh

  ld   a,i           ; ED 57
  ld   r,a           ; ED 4F
  ld   a,r           ; ED 5F
  ld   i,a           ; ED 47

  ld   ix,49152      ; DD 21 NNl NNh
  ld   bc,49152      ; 01 NNl NNh

  ld   bc,(49152)    ; ED 4B NNl NNh
  ld   ix,(49152)    ; DD 2A NNl NNh
  ld   hl,(49152)    ; 2A NNl NNh

  ld   (49152),bc    ; ED 43 NNl NNh
  ld   (49152),ix    ; DD 22 NNl NNh
  ld   (49152),hl    ; 22 NNl NNh

  ld   sp,hl         ; F9
  ld   sp,ix         ; DD F9
  ld   sp,iy         ; FD F9

  or   (hl)          ; B6
  or   (ix)          ; DD B6 00
  or   (ix+10)      ; DD B6 DIS
  or   (ix-10)     ; DD B6 NDIS
  or   (iy)          ; FD B6 00
  or   (iy+10)      ; FD B6 DIS
  or   (iy-10)     ; FD B6 NDIS
  or   10             ; F6 N
  or   a             ; B7
  or   b             ; B0
  or   c             ; B1
  or   d             ; B2
  or   e             ; B3
  or   h             ; B4
  or   ixh           ; DD B4
  or   ixl           ; DD B5
  or   iyh           ; FD B4
  or   iyl           ; FD B5
  or   l             ; B5

  out  (10),a        ; D3 N
  out  (c),0         ; ED 71
  out  (c),h         ; ED 61
  out  (c),l         ; ED 69
  outd               ; ED AB
  outi               ; ED A3

  push af            ; F5
  push bc            ; C5
  push de            ; D5
  push hl            ; E5
  push ix            ; DD E5
  push iy            ; FD E5
  pop  af            ; F1
  pop  bc            ; C1
  pop  de            ; D1
  pop  hl            ; E1
  pop  ix            ; DD E1
  pop  iy            ; FD E1

  bit  6,l           ; CB 75
  bit  7,(hl)        ; CB 7E
  bit  7,(ix)        ; DD CB 00 7E
  bit  7,(ix+5)      ; DD CB DIS 7E

  ret                ; C9
  ret  c             ; D8
  ret  m             ; F8
  ret  nc            ; D0
  ret  nz            ; C0
  ret  p             ; F0
  ret  pe            ; E8
  ret  po            ; E0
  ret  z             ; C8

  rl   c             ; CB 11
  rl   d             ; CB 12
  rl   (hl)          ; CB 16

  rst  0             ; C7
  rst  16            ; D7
  rst  24            ; DF

  res  6,(hl)        ; CB B6

  res  7,b           ; CB B8
  res  7,c           ; CB B9
  res  7,d           ; CB BA

  res  7,(ix+10),c  ; DD CB DIS B9
  res  7,(ix+10),d  ; DD CB DIS BA
  res  7,(ix+10),e  ; DD CB DIS BB
  res  5,(iy-10)    ; FD CB NDIS AE

  set  7,(iy),c      ; FD CB 00 F9
  set  7,(iy),d      ; FD CB 00 FA
  set  7,(iy),e      ; FD CB 00 FB
  set  7,(ix+10),b  ; DD CB DIS F8
  set  7,(ix+10),c  ; DD CB DIS F9
  set  7,(ix+10),d  ; DD CB DIS FA
  set  7,(iy)        ; FD CB 00 FE
  set  5,c           ; CB E9
  set  5,d           ; CB EA



