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
  stop               ; DD DD 00

  adc  a,(hl)        ; 8E

  sub  (hl)          ; 96
  sub  5             ; D6 N

  ind                ; ED AA
  indr               ; ED BA

  bit  6,l           ; CB 75
  bit  7,(hl)        ; CB 7E
  bit  7,(ix)        ; DD CB 00 7E
  bit  7,(ix+5)      ; DD CB DIS 7E

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

