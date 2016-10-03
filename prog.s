.data
a: .word 25
b: .word 15

.text
main:
la $t0, a
lw $t1, 0($t0)
la $t2, b
lw $t3, 0($t2)
