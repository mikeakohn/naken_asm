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
  jr   10            ; 18 NNo
  jr   c,10          ; 38 NNo
  ;jr   m,10          ; FA NNl NNh
  jr   nc,10         ; 30 NNo
  jr   nz,10         ; 20 NNo
  ;jr   p,10          ; F2 NNl NNh
  ;jr   pe,10         ; EA NNl NNh
  ;jr   po,10         ; E2 NNl NNh
  jr   z,10          ; 28 NNo

  ld   l,a           ; 6F
  ld   l,b           ; 68
  ld   l,c           ; 69
  ld   l,d           ; 6A
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

.if 0
  ld   a,i           ; ED 57
  ld   (10),bc       ; ED 43 NNl NNh
  ld   (10),de       ; ED 53 NNl NNh
  ld   (10),hl       ; 22 NNl NNh
  ld   (10),ix       ; DD 22 NNl NNh
  ld   (10),iy       ; FD 22 NNl NNh
  ld   (10),sp       ; ED 73 NNl NNh
  ld   (hl),bc       ; 71 23 70 2B
  ld   (hl),c        ; 71
  ld   (hl),d        ; 72
  ld   (hl),de       ; 73 23 72 2B
  ld   (ix),N        ; DD 36 00 N
  ld   (ix),a        ; DD 77 00
  ld   (ix),b        ; DD 70 00
  ld   (ix),bc       ; DD 71 00 DD 70 01
  ld   (ix),c        ; DD 71 00
  ld   (ix),d        ; DD 72 00
  ld   (ix),de       ; DD 73 00 DD 72 01
  ld   (ix),e        ; DD 73 00
  ld   (ix),h        ; DD 74 00
  ld   (ix),hl       ; DD 75 00 DD 74 01
  ld   (ix),l        ; DD 75 00
  ld   (ix+12),bc   ; DD 71 DIS DD 70 DIS+1
  ld   (ix+12),c    ; DD 71 DIS
  ld   (ix+12),d    ; DD 72 DIS
  ld   (ix+12),de   ; DD 73 DIS DD 72 DIS+1
  ld   (ix+12),e    ; DD 73 DIS
  ld   (ix+12),h    ; DD 74 DIS
  ld   (ix+12),hl   ; DD 75 DIS DD 74 DIS+1
  ld   (ix+12),l    ; DD 75 DIS
  ld   (ix-11),N   ; DD 36 N12 N
  ld   (ix-11),a   ; DD 77 N12
  ld   (ix-11),b   ; DD 70 N12
  ld   (ix-11),bc  ; DD 71 N12 DD 70 NDIS+1
  ld   (ix-11),c   ; DD 71 N12
  ld   (ix-11),d   ; DD 72 N12
  ld   (ix-11),de  ; DD 73 N12 DD 72 NDIS+1
  ld   (ix-11),e   ; DD 73 N12
  ld   (ix-11),h   ; DD 74 N12
  ld   (ix-11),hl  ; DD 75 N12 DD 74 NDIS+1
  ld   (ix-11),l   ; DD 75 N12
  ld   (iy),N        ; FD 36 00 N
  ld   (iy),a        ; FD 77 00
  ld   (iy),b        ; FD 70 00
  ld   (iy),bc       ; FD 71 00 FD 70 01
  ld   (iy),c        ; FD 71 00
  ld   (iy),d        ; FD 72 00
  ld   (iy),de       ; FD 73 00 FD 72 01
  ld   (iy),e        ; FD 73 00
  ld   (iy),h        ; FD 74 00
  ld   (iy),hl       ; FD 75 00 FD 74 01
  ld   (iy),l        ; FD 75 00
  ld   (iy+12),N    ; FD 36 DIS N
  ld   (iy+12),a    ; FD 77 DIS
  ld   (iy+12),b    ; FD 70 DIS
  ld   (iy+12),bc   ; FD 71 DIS FD 70 DIS+1
  ld   (iy+12),c    ; FD 71 DIS
  ld   (iy+12),d    ; FD 72 DIS
  ld   (iy+12),de   ; FD 73 DIS FD 72 DIS+1
  ld   (iy+12),e    ; FD 73 DIS
  ld   (iy+12),h    ; FD 74 DIS
  ld   (iy+12),hl   ; FD 75 DIS FD 74 DIS+1
  ld   (iy+12),l    ; FD 75 DIS
  ld   (iy-11),N   ; FD 36 N12 N
  ld   (iy-11),bc  ; FD 71 N12 FD 70 NDIS+1
  ld   (iy-11),c   ; FD 71 N12
  ld   (iy-11),d   ; FD 72 N12
  ld   (iy-11),de  ; FD 73 N12 FD 72 NDIS+1
  ld   (iy-11),e   ; FD 73 N12
  ld   (iy-11),h   ; FD 74 N12
  ld   (iy-11),hl  ; FD 75 N12 FD 74 NDIS+1
  ld   (iy-11),l   ; FD 75 N12
  ld   a,(hl)        ; 7E
  ld   a,(ix)        ; DD 7E 00
  ld   a,N           ; 3E N
  ld   a,r           ; ED 5F
  ld   b,(ix)        ; DD 46 00
  ld   b,(ix+12)    ; DD 46 DIS
  ld   b,(ix-11)   ; DD 46 N12
  ld   b,(iy)        ; FD 46 00
  ld   b,(iy+12)    ; FD 46 DIS
  ld   b,(iy-11)   ; FD 46 N12
  ld   b,N           ; 06 N
  ld   bc,(10)       ; ED 4B NNl NNh
  ld   bc,(hl)       ; 4E 23 46 2B
  ld   bc,(ix)       ; DD 4E 00 DD 46 01
  ld   bc,(ix+12)   ; DD 4E DIS DD 46 DIS+1
  ld   bc,(ix-11)  ; DD 4E N12 DD 46 NDIS+1
  ld   bc,(iy)       ; FD 4E 00 FD 46 01
  ld   bc,(iy+12)   ; FD 4E DIS FD 46 DIS+1
  ld   bc,(iy-11)  ; FD 4E N12 FD 46 NDIS+1
  ld   bc,10         ; 01 NNl NNh
  ld   bc,bc         ; 40 49
  ld   bc,de         ; 42 4B
  ld   bc,hl         ; 44 4D
  ld   bc,ix         ; DD 44 DD 4D
  ld   bc,iy         ; FD 44 FD 4D
  ld   c,(hl)        ; 4E
  ld   c,(ix)        ; DD 4E 00
  ld   c,(ix+12)    ; DD 4E DIS
  ld   c,(ix-11)   ; DD 4E N12
  ld   c,(iy)        ; FD 4E 00
  ld   c,(iy+12)    ; FD 4E DIS
  ld   c,(iy-11)   ; FD 4E N12
  ld   c,ixh         ; DD 4C
  ld   c,ixl         ; DD 4D
  ld   c,iyh         ; FD 4C
  ld   c,iyl         ; FD 4D
  ld   d,(hl)        ; 56
  ld   d,(ix)        ; DD 56 00
  ld   d,(ix+12)    ; DD 56 DIS
  ld   d,(ix-11)   ; DD 56 N12
  ld   d,(iy)        ; FD 56 00
  ld   d,(iy+12)    ; FD 56 DIS
  ld   d,(iy-11)   ; FD 56 N12
  ld   d,N           ; 16 N
  ld   d,ixh         ; DD 54
  ld   d,ixl         ; DD 55
  ld   d,iyh         ; FD 54
  ld   d,iyl         ; FD 55
  ld   de,(10)       ; ED 5B NNl NNh
  ld   de,(hl)       ; 5E 23 56 2B
  ld   de,(ix)       ; DD 5E 00 DD 56 01
  ld   de,(ix+12)   ; DD 5E DIS DD 56 DIS+1
  ld   de,(ix-11)  ; DD 5E N12 DD 56 NDIS+1
  ld   de,(iy)       ; FD 5E 00 FD 56 01
  ld   de,(iy+12)   ; FD 5E DIS FD 56 DIS+1
  ld   de,(iy-11)  ; FD 5E N12 FD 56 NDIS+1
  ld   de,10         ; 11 NNl NNh
  ld   de,bc         ; 50 59
  ld   de,de         ; 52 5B
  ld   de,hl         ; 54 5D
  ld   de,ix         ; DD 54 DD 5D
  ld   de,iy         ; FD 54 FD 5D
  ld   e,(hl)        ; 5E
  ld   e,(ix)        ; DD 5E 00
  ld   e,(ix+12)    ; DD 5E DIS
  ld   e,(ix-11)   ; DD 5E N12
  ld   e,(iy)        ; FD 5E 00
  ld   e,(iy+12)    ; FD 5E DIS
  ld   e,(iy-11)   ; FD 5E N12
  ld   e,N           ; 1E N
  ld   e,ixh         ; DD 5C
  ld   e,ixl         ; DD 5D
  ld   e,iyh         ; FD 5C
  ld   e,iyl         ; FD 5D
  ld   h,(ix)        ; DD 66 00
  ld   h,(ix+12)    ; DD 66 DIS
  ld   h,(ix-11)   ; DD 66 N12
  ld   h,(iy)        ; FD 66 00
  ld   h,(iy+12)    ; FD 66 DIS
  ld   h,(iy-11)   ; FD 66 N12
  ld   h,N           ; 26 N
  ld   hl,(10)       ; 2A NNl NNh
  ld   hl,(ix)       ; DD 6E 00 DD 66 01
  ld   hl,(ix+12)   ; DD 6E DIS DD 66 DIS+1
  ld   hl,(ix-11)  ; DD 6E N12 DD 66 NDIS+1
  ld   hl,(iy)       ; FD 6E 00 FD 66 01
  ld   hl,(iy+12)   ; FD 6E DIS FD 66 DIS+1
  ld   hl,(iy-11)  ; FD 6E N12 FD 66 NDIS+1
  ld   hl,10         ; 21 NNl NNh
  ld   hl,bc         ; 60 69
  ld   hl,de         ; 62 6B
  ld   hl,hl         ; 64 6D
  ld   hl,ix         ; DD E5 E1
  ld   hl,iy         ; FD E5 E1
  ld   i,a           ; ED 47
  ld   ix,(10)       ; DD 2A NNl NNh
  ld   ix,10         ; DD 21 NNl NNh
  ld   ix,bc         ; DD 69 DD 60
  ld   ix,de         ; DD 6B DD 62
  ld   ix,hl         ; E5 DD E1
  ld   ix,ix         ; DD 6D DD 64
  ld   ix,iy         ; FD E5 DD E1
  ld   ixh,N         ; DD 26 N
  ld   ixh,a         ; DD 67
  ld   ixh,b         ; DD 60
  ld   ixh,c         ; DD 61
  ld   ixh,d         ; DD 62
  ld   ixh,e         ; DD 63
  ld   ixl,N         ; DD 2E N
  ld   iy,(10)       ; FD 2A NNl NNh
  ld   iy,10         ; FD 21 NNl NNh
  ld   iy,bc         ; FD 69 FD 60
  ld   iy,de         ; FD 6B FD 62
  ld   iy,hl         ; E5 FD E1
  ld   iy,ix         ; DD E5 FD E1
  ld   iy,iy         ; FD 6D FD 64
  ld   iyh,N         ; FD 26 N
  ld   iyh,a         ; FD 67
  ld   iyh,b         ; FD 60
  ld   iyh,c         ; FD 61
  ld   iyh,d         ; FD 62
  ld   iyh,e         ; FD 63
  ld   iyh,iyh       ; FD 64
  ld   iyh,iyl       ; FD 65
  ld   iyl,N         ; FD 2E N
  ld   iyl,a         ; FD 6F
  ld   iyl,b         ; FD 68
  ld   iyl,c         ; FD 69
  ld   iyl,d         ; FD 6A
  ld   iyl,e         ; FD 6B
  ld   iyl,iyh       ; FD 6C
  ld   iyl,iyl       ; FD 6D
  ld   l,(hl)        ; 6E
  ld   l,(ix)        ; DD 6E 00
  ld   l,(ix+12)    ; DD 6E DIS
  ld   l,(ix-11)   ; DD 6E N12
  ld   l,(iy)        ; FD 6E 00
  ld   l,(iy+12)    ; FD 6E DIS
  ld   l,(iy-11)   ; FD 6E N12
  ld   l,N           ; 2E N
  ld   r,a           ; ED 4F
  ld   sp,(10)       ; ED 7B NNl NNh
  ld   sp,10         ; 31 NNl NNh
  ld   sp,hl         ; F9
  ld   sp,ix         ; DD F9
  ld   sp,iy         ; FD F9
.endif



