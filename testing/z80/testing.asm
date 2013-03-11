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

  sbc  a,ixh         ; DD 9C
  sbc  a,ixl         ; DD 9D
  sbc  a,iyh         ; FD 9C
  sbc  a,iyl         ; FD 9D



