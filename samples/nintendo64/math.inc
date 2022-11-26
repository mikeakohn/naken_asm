
;; Test 4.0 / 2.0.
;li $t8, 0x4_0000
;sw $t8, 8($k0)
;li $t8, 0x2_0000
;sw $t8, 12($k0)
;DIVIDE_I_IF

.macro DIVIDE_I_IF
  ;; dividend / divisor = quotient becomes
  ;; (1 / divisor) * dividend = quotient

  ;; Set $v12 to a 16.16 fixed point number (divisor).
  llv $v12[0], 12($k0)

  ;; From the manual to compute the reciprocal:
  ;; vrcph res_int[0],  s_int[0]
  ;; vrcpl res_frac[0], s_frac[0]
  ;; vrcph res_int[0],  dev_null[0]
  vrcph $v20[0], $v12[0]
  vrcpl $v21[0], $v12[1]
  vrcph $v20[0], $v0[0]

  ;; From the manual, since the reciprocal is S15.16 fixed point, convert
  ;; it to 16.16 by multiplying by 2.0.
  ;; vmudn res_frac, s_frac,    vconst_2[0] # constant of 2
  ;; vmadm res_int,  s_int,     vconst_2[0]
  ;; vmadn res_frac, dev_null,  dev_null[0]
  ;vmudn $v23, $v21, $v2[0]
  ;vmadm $v22, $v20, $v2[0]
  ;vmadn $v23, $v0,  $v0[0]

  ;; The above code for multiplying the reciprocal by 2 doesn't seem
  ;; to work with signed numbers. The code below does. Sometime later
  ;; need to figure out why.
  ssv $v20[0], 12($k0)
  ssv $v21[0], 14($k0)
  lw $v0, 8($k0)
  sll $v0, $v0, 1
  sw $v0, 8($k0)
  lsv $v22[0], 12($k0)
  lsv $v23[0], 14($k0)

  ;; Load $v12[0] with the dividend.
  llv $v12[0], 8($k0)

  ;; IF * I = IF
  ;; vmudn res_frac, s_frac,   dividend_int[0]
  ;; vmadh res_int,  s_int,    dividend_int[0]
  ;; vmadn res_frac, dev_null, dev_null[0]
  vmudn $v25, $v23, $v12[0]
  vmadh $v24, $v22, $v12[0]
  vmadn $v25, $v0,  $v0[0]

  ;; Copy result back to DMEM.
  ssv $v24[0], 8($k0)
  ssv $v25[0], 10($k0)
.endm

.macro DIVIDE_I_IF_UNSIGNED
  ;; dividend / divisor = quotient becomes
  ;; (1 / divisor) * dividend = quotient

  ;; Set $v12 to a 16.16 fixed point number (divisor).
  llv $v12[0], 12($k0)

  ;; From the manual to compute the reciprocal:
  ;; vrcph res_int[0],  s_int[0]
  ;; vrcpl res_frac[0], s_frac[0]
  ;; vrcph res_int[0],  dev_null[0]
  vrcph $v20[0], $v12[0]
  vrcpl $v21[0], $v12[1]
  vrcph $v20[0], $v0[0]

  ;; From the manual, since the reciprocal is S15.16 fixed point, convert
  ;; it to 16.16 by multiplying by 2.0.
  ;; vmudn res_frac, s_frac,    vconst_2[0] # constant of 2
  ;; vmadm res_int,  s_int,     vconst_2[0]
  ;; vmadn res_frac, dev_null,  dev_null[0]
  vmudn $v23, $v21, $v2[0]
  vmadm $v22, $v20, $v2[0]
  vmadn $v23, $v0,  $v0[0]

  ;; Load $v12[0] with the dividend.
  llv $v12[0], 8($k0)

  ;; IF * I = IF
  ;; vmudn res_frac, s_frac,   dividend_int[0]
  ;; vmadh res_int,  s_int,    dividend_int[0]
  ;; vmadn res_frac, dev_null, dev_null[0]
  vmudn $v25, $v23, $v12[0]
  vmadh $v24, $v22, $v12[0]
  vmadn $v25, $v0,  $v0[0]

  ;; Copy result back to DMEM.
  ssv $v24[0], 8($k0)
  ssv $v25[0], 10($k0)
.endm

;; IF * I = IF
.macro MULTIPLY_IFxI
  lsv $v22[0], 8($k0)  ; s_int
  lsv $v23[0], 10($k0) ; s_frac
  llv $v12[0], 12($k0) ; t_int / t_frac

  ;; vmudn res_frac, s_frac,   t_int
  ;; vmadh res_int,  s_int,    t_int
  ;; vmadn res_frac, dev_null, dev_null[0]
  vmudn $v25, $v23, $v12[0]
  vmadh $v24, $v22, $v12[0]
  vmadn $v25, $v0,  $v0[0]

  ;; Copy result back to DMEM.
  ssv $v24[0], 8($k0)
  ssv $v25[0], 10($k0)
.endm

;; I * IF = IF
.macro MULTIPLY_IxIF
  lsv $v22[0], 8($k0)  ; s_int
  llv $v12[0], 12($k0) ; t_int / t_frac

  ;; vmudm res_frac, s_int,    t_frac
  ;; vmadh res_int,  s_int,    t_int
  ;; vmadn res_frac, dev_null, dev_null[0]
  vmudm $v25, $v22, $v12[1]
  vmadh $v24, $v22, $v12[0]
  vmadn $v25, $v0,  $v0[0]

  ;; Copy result back to DMEM.
  ssv $v24[0], 8($k0)
  ssv $v25[0], 10($k0)
.endm

;; IF * F = IF
.macro MULTIPLY_IFxF
  lsv $v22[0], 8($k0)  ; s_int
  lsv $v23[0], 10($k0) ; s_frac
  llv $v12[0], 12($k0) ; t_int / t_frac

  ;; vmudl res_frac, s_frac,   t_frac
  ;; vmadh res_int,  s_int,    t_frac
  ;; vmadn res_frac, dev_null, dev_null[0]
  vmudl $v25, $v23, $v12[1]
  vmadm $v24, $v22, $v12[1]
  vmadn $v25, $v0,  $v0[0]

  ;; Copy result back to DMEM.
  ssv $v24[0], 8($k0)
  ssv $v25[0], 10($k0)
.endm

;; I * I = I
.macro MULTIPLY_IxI
  lsv $v22[0], 8($k0)  ; s_int
  lsv $v12[0], 12($k0) ; t_int

  ;; vmudh res_int, s_int, t_int
  vmudh $v24, $v22, $v12[0]

  ;; Copy result back to DMEM.
  ssv $v24[0], 8($k0)
  sh $0, 10($k0)
.endm

;; I * F = IF
.macro MULTIPLY_IxF
  lsv $v22[0], 8($k0)  ; s_int
  llv $v12[0], 12($k0) ; t_int / t_frac

  ;; vmudm res_int,  s_int,    t_frac
  ;; vmadn res_frac, dev_null, dev_null[0]
  vmudm $v24, $v22, $v12[1]
  vmadn $v25, $v0,  $v0[0]

  ;; Copy result back to DMEM.
  ssv $v24[0], 8($k0)
  ssv $v25[0], 10($k0)
.endm

