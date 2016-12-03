.data
N: .word 5
X: .word 2, 4, 6, 18, 10
SUM: .word 0

.globl main

.text

main:
lw $s0, N
la $t0, X
and $s1, $s1, $zero
loop: lw $t1, 0($t0)
add $s1, $s1, $t1
addi $t0, $t0, 4
addi $s0, $s0, -1
beq $0, $s0, exit
j loop
exit:
sw $s1, SUM

halt