.data
A: .word 20, -13
B: .word 25

.globl main
.text
main:
	lw $t0, A
	lw $t1, B
	add $t2, $t0, $t1
	addi $t3, $t0, -7
    sub $t4, $t0, $t1
    mul $t5, $t0, $t1
    and $t6, $t0, $t1
    andi $t7, $t0, -7
    or $s0, $t0, $t1
    ori $s1, $t0, -7
    nor $s2, $t0, $t1
    slt $s3, $t0, $t1
    slti $s4, $t0, 50
halt
