.webasm

.dc8 0, "asm"
.dc32 1


;; Code section (10)
.varuint 10
.varuint32 code_end - start

start:
i32.add
i32.sub
code_end:

