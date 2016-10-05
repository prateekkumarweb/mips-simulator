#sdgdgds
.data
a: .word 25
be: .word 15

.text
main:
la $t0, a
lw $t1, 0($t0)
la $t2, be

lw $t1, 00($t0)
lw $t3, -4($t2)
#li $v0 10
#syscall