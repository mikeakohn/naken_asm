
  ;; Here's a piece of code for doing division based on code from page 79
  ;; from the SGI Nintendo 64 RSP Programmer's Guide.

  ;; Set $k0 to a scratchpad area of DMEM.
  li $k0, 0xff0

  ;; Set $v0 to a vector of 0's.
  vxor $v0, $v0, $v0

  ;; Set $v2 to a vector of 2's.
  li $k1, 0x0002
  sh $k1, 8($k0)
  llv $v2[0], 8($k0)
  llv $v2[2], 8($k0)
  llv $v2[4], 8($k0)
  llv $v2[6], 8($k0)
  llv $v2[8], 8($k0)
  llv $v2[10], 8($k0)
  llv $v2[12], 8($k0)
  llv $v2[14], 8($k0)

  ;; Set $v12 to a 16.16 fixed point number.
  ;; Upper 16 bits is integer $v12[0] and lower is fraction $v12[2].
  ;; 0x0000_4000 is 0.2500 (0 = whole, 0x4000 / 65536 = fraction = 0.25).
  li $k1, 0x4000
  sw $k1, 8($k0)
  llv $v12[0], 8($k0)

  ;; From the manual to compute the reciprocal:
  ;; vrcph sres_int[0],  s_int[0]
  ;; vrcpl sres_frac[0], s_frac[0]
  ;; vrcph sres_int[0],  dev_null[0]
  vrcph $v20[0], $v12[0]
  vrcpl $v21[0], $v12[1]
  vrcph $v20[0], $v0[0]

  ;; From the manual, since the reciprocal is S15.16 fixed point, convert
  ;; it to 16.16 by multiplying by 2.0.
  ;; vmudn sres_frac, sres_frac, vconst[2] # constant of 2
  ;; vmadm sres_int,  sres_int,  vconst[2]
  ;; vmadn sres_frac, dev_null,  dev_null[0]
  vmudn $v23, $v21, $v2[0]
  vmadm $v22, $v20, $v2[0]
  vmadn $v23, $v0,  $v0[0]

