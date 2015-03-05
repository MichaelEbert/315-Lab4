#Circle algorithms in MIPS assember
#parameters: xc, yc, r
#xc = x center, yc = y center, r = radius

#circle expects 3 arguments (arguments explained above):
#$a0 = xc
#$a1 = yc
#$a2 = r

#$s0 = xc
#$s1 = yc
#$s2 = x
#$s3 = y
#$s4 = g
#$s5 = diagonalInc
#$s6 = rightInc
#$t0 = temp

#$s7 = next address to plot to

#main is the last function in the program, so the program will exit when it completes.
	j main
circle:
	#save previous registers
	addi $sp, $sp, -32
	sw $s0, 0($sp)
	sw $s1, 4($sp)
	sw $s2, 8($sp)
	sw $s3, 12($sp)
	sw $s4, 16($sp)
	sw $s5, 20($sp)
	sw $s6, 24($sp)
	sw $ra, 28($sp)
	#initialize our registers
	#x = 0, y = r
	addi $s2, $0, 0 ##line 10
	addi $s3, $a2, 0
	#g = 3 - 2*r
	add $s4, $a2, $a2
	#premultiply for diagonalInc
	add $s5, $s4, $s4
	#continue with g
	addi $t0, $0, 3
	sub $s4, $t0, $s4
	#diagonalInc
	addi $t0, $0, 10
	sub $s5, $t0, $s5
	#rightInc
	addi $s6, $0, 6
	#xc and yc
	addi $s0, $a0, 0
	addi $s1, $a1, 0 ##line 20

#x Lessthan or EQualto y, which is the same as !(x>y)
xleqy:	slt $t0, $s3, $s2
	bne $t0, $0, xend
	
    #the plots
	add $a0, $s0, $s2
	add $a1, $s1, $s3
	jal plot
	sub $a1, $s1, $s3
	jal plot

	sub $a0, $s0, $s2
	add $a1, $s1, $s3
	jal plot ##line 30
	sub $a1, $s1, $s3
	jal plot
	
	add $a0, $s0, $s3
	add $a1, $s1, $s2
	jal plot
	sub $a1, $s1, $s2
	jal plot

	sub $a0, $s0, $s3
	add $a1, $s1, $s2
	jal plot ##line 40
	sub $a1, $s1, $s2
	jal plot

	##if g >= 0## (g not < 0)
	slt $t0, $s4, $0
	bne $t0, $0, gltz
	##g+= diagonalInc##
	add $s4, $s4, $s5 ##line 45
	##diagonalInc+=8##
	addi $s5, $s5, 8
	##y-=1##
	addi $s3, $s3, -1
	j gend
    ##else##
gltz:	add $s4, $s4, $s6
	addi $s5, $s5, 4##line 50

#end of if(g>=0) block
gend:	addi $s6, $s6, 4
	addi $s2, $s2, 1
	j xleqy

#end of x<=y loop
xend:
#end: restore registers
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
	addi $s7, $s7, 8
	jr $ra
	
main:	addi $s7, $0, 0
    addi $sp, $0, 8192

	addi $a0, $0, 30
	addi $a1, $0, 100
	addi $a2, $0, 20
	jal circle
	#need fake instruction to correctly run
	sll $0, $0, 0
