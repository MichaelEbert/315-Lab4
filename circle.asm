;Circle algorithms in MIPS assember
;parameters: xc, yc, r
;xc = x center, yc = y center, r = radius

x = 0;
y = r;
g = 3 -2*r
diagonalInc = 10-4*r
rightInc = 6
while(x <= y){
plot(xc+x, yc+y)
plot(xc+x, yc - y)
plot(xc-x,yc+y
xc-x,yc-y
xc+y
+y
-y
-y
}
;and then more stuff here
}
$a0 = xc
$a1 = yc
$a2 = r

$s0 = xc;
$s1 = yc;
$s2 = x;
$s3 = y;
$s4 = g;
$s5 = diagonalInc;
$s6 = rightInc;
$t0 = temp;

$s7 = next address to plot to

circle:
	;save previous registers
	addi $sp, $sp, -32
	sw $s0, 0($sp)
	sw $s1, 4($sp)
	sw $s2, 8($sp)
	sw $s3, 12($sp)
	sw $s4, 16($sp)
	sw $s5, 20($sp)
	sw $s6, 24($sp)
	sw $ra, 28($sp)
	;initialize our registers
	addi $s2, $s2, 0
	addi $s3, $a2, 0
	;g = 3 - 2*r
	add $s4, $a2, $a2
	;premultiply for diagonalInc
	add $s5, $s4, $s4
	;continue with g
	addi $t0, $0, 3
	sub $s4, $t0, $s4
	;diagonalInc
	addi $t0, $0, 10
	sub $t3, $t0, $t3
	;rightInc
	addi $s6, $0, 6
	;xc and yc
	addi $s0, $a0, 0
	addi $s1, $a1, 0

;x <= y === y >= x
xleqy:	slt $t0, $s3, $s2
	bne $t0, $0, xend

	add $a0, $a0, $s2
	add $a1, $a1, $s3
	jal plot
	sub $a1, $s1, $s3
	jal plot

	sub $a0, $s0, $s2
	add $a1, $s1, $s3
	jal plot
	sub $a1, $s1, $s3
	jal plot
	
	add $a0, $s0, $s3
	add $a1, $s1, $s2
	jal plot
	sub $a1, $s1, $s2
	jal plot

	sub $a0, $s0, $s3
	add $a1, $s1, $s2
	jal plot
	sub $a1, $s1, $s2
	jal plot

	;if g >= 0: g+= diagonalInc
	slti $t0, $s4, 0
	bne $t0, $0, gltz
	add $s4, $s4, $s5 
	;diagonalInc+=8
	addi $s5, $s5, 8
	;y-=1
	addi $s3, $s3, -1
	br gend
;else
gltz:	add $s4, $s4, $s6
	addi $s5, $s5, 4

;end if(g>=0) block
gend:	addi $s6, $s6, 4
	addi $s2, $s2, 1

;end x<=y loop
xend:
;end: restore registers
	lw $s0, 0($sp)
	lw $s1, 4($sp)
	lw $s2, 8($sp)
	lw $s3, 12($sp)
	lw $s4, 16($sp)
	lw $s5, 20($sp)
	lw $s6, 24($sp)
	lw $ra, 28($sp)
	addi $sp, $sp, 32
	jr $ra

plot:
	sw $a0, 0($s7)
	sw $a1, 4($s7)
	addi $s7, 8
	jr $ra
	
