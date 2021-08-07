.arm64

abs v9.16b, v18.16b
add w10, w17, w9, lsl #3
add w31, w17, w9
add w10, w17, w9, uxth
add x10, x17, x9, sxtb
add x10, x11, #0x123
add x10, x11, #0x123000
add w10, w11, #0x123, lsl #12
add x11, x28, x1, lsl #3
add x11, x28, x1, asr #10
add v9.16b, v18.16b, v11.16b
add v28.4h, v18.4h, v5.4h
add d17, d1, d29
addg x5, x9, #32, #5
;addp v3.d, v7.2d fixme
;addv fixme
aese v7.16b, v13.16b
at s1e0w, x13

