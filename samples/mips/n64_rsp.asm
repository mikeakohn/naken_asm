.n64_rsp

li $v1, 234
add $v1, $v0, $t0
lsv $v1[14], 16($1)
lsv $v1[12], 16($1)
lhv $v1[0], 256($1)
slv $v1[0], 128($1)
mtc2 $t5, $v4[4]
mfc2 $t5, $v31[4]
vnop
vmov $v3[15], $v5[2]
vmrg $v2, $v9, $v7
vmrg $v9, $v8, $v4[1q]
vmrg $v31, $v15, $v4[2h]
vmrg $v7, $v9, $v4[1]
vmrg $v7, $v9, $v0[1]
vmrg $v1, $v5, $v3[1q]
vadd $v0, $v5, $v0

lsv $v16[8], -28($27)
llv $v23[4], -20($7)

