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

