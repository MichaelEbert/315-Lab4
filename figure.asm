# MAIN
add	$s7, $0, $0
addi	$sp, $0, 8191

addi	$a0, $0, 30	#head
addi	$a1, $0, 100
addi	$a2, $0, 20
jal	circle

addi	$a0, $0, 30	#body
addi	$a1, $0, 80
addi	$a2, $0, 30
addi	$a3, $0, 30
jal	line

addi	$a0, $0, 20	#left leg
addi	$a1, $0, 1
addi	$a2, $0, 30
addi	$a3, $0, 30
jal	line

addi	$a0, $0, 40	#right leg
addi	$a1, $0, 1
addi	$a2, $0, 30
addi	$a3, $0, 30
jal	line

addi	$a0, $0, 15	#left arm
addi	$a1, $0, 60
addi	$a2, $0, 30
addi	$a3, $0, 50
jal	line

addi	$a0, $0, 30	#right arm
addi	$a1, $0, 50
addi	$a2, $0, 45
addi	$a3, $0, 60
jal	line

addi	$a0, $0, 24	#left eye
addi	$a1, $0, 105
addi	$a2, $0, 3
jal	circle

addi	$a0, $0, 36	#right eye
addi	$a1, $0, 105
addi	$a2, $0, 3
jal	circle

addi	$a0, $0, 25	#mouth center
addi	$a1, $0, 90
addi	$a2, $0, 35
addi	$a3, $0, 90
jal	line

addi	$a0, $0, 25	#mouth left
addi	$a1, $0, 90
addi	$a2, $0, 20
addi	$a3, $0, 95
jal	line

addi	$a0, $0, 35	#mouth right
addi	$a1, $0, 90
addi	$a2, $0, 40
addi	$a3, $0, 95
jal	line

j	end




#####################################################################################
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
	


#####################################################################################

line: 	sub	$s0, $a3, $a1	# $s0 holds y1 - y0
	sub	$s1, $a2, $a0	# $s1 holds x1 - x0
	

	###### Computing abs value of y1 - y0 ######
	add	$t0, $s0, $0	# copies $s0 into $t0
	slt	$t1, $s0, $0	# is $s0 < 0?
	beq	$t1, $0, ypos	# branch if positive
	sub	$t0, $0, $s0	# 0 - $s0 if negative	
ypos:	add	$s0, $t0, $0	# abs($s0) computed


	###### Computing abs value of x1 - x0 ######
	add	$t0, $s1, $0	# copies $s1 into $t0
	slt	$t1, $s1, $0	# is $s1 < 0?
	beq	$t1, $0, xpos	# branch if positive
	sub	$t0, $0, $s1	# 0 - $s1 if negative
xpos:	add	$s1, $t0, $0	# abs($s1) computed


	###### if abs(y1 - y0) > abs(x1 - x0)  ######
	slt	$t0, $s1, $s0	# is $s1 < $s0
	add	$s2, $0, $0	# $s2 is st, $s2 = 0
	beq	$t0, $0, else0	# branch if else case
	addi	$s2, $0, 1	# $s2 = 1 if we fall through


	###### if st == 1 and swapping ######
else0:	addi	$t0, $0, 1	# $t0 = 1
	bne	$s2, $t0, else1	# do not swap
	add	$t0, $a0, $0	# $t0 = $a0
	add	$a0, $a1, $0	# $a0 = $a1
	add	$a1, $t0, $0	# $a1 = $t0
	add	$t0, $a2, $0	# $t0 = $a2
	add	$a2, $a3, $0	# $a2 = $a3
	add	$a3, $t0, $0	# $a3 = $t0


	###### if x0 > x1 and swapping #######
else1:	slt	$t0, $a2, $a0	# is x0 > x1?
	beq	$t0, $0, else2	# do not swap
	add	$t0, $a0, $0	# $t0 = $a0
	add	$a0, $a2, $0	# $a0 = $a2
	add	$a2, $t0, $0	# $a2 = $t0
	add	$t0, $a1, $0	# $t0 = $a1
	add	$a1, $a3, $0	# $a1 = $a3
	add	$a3, $t0, $0	# $a3 = $t0

else2:	sub	$s0, $a2, $a0	# $s0 = deltax = x1 - x0
	sub	$s1, $a3, $a1	# $s1 = y1 - y0

	
	###### Computing abs value for deltay ######
	add	$t0, $s1, $0	# copies $s1 into $t0
	slt	$t1, $s1, $0	# is $s1 < 0?
	beq	$t1, $0, ypos2	# branch if positive
	sub	$t0, $0, $s1	# 0 - $s0 if negative
ypos2:	add	$s1, $t0, $0	# $s1 = deltay = abs(y1 - y0)


	add	$s3, $0, $0	# $s3 = error = 0
	add	$s4, $a1, $0	# $s4 = y = y0

	
	###### if y0 < y1 ######
	slt	$t0, $a1, $a3	# is $a1 < $a3 (y0 < y1)
	addi	$s5, $0, 1	# $s5 = ystep = 1
	bne	$t0, $0, else3	# branch if y0 < y1 (skip else)
	addi	$s5, $0, -1	# $s5 = ystep = -1


	###### For loop ######
else3:	add	$s6, $a0, $0	# $s6 = x = x0
	addi	$t7, $a2, 1	# $t7 = x1 + 1

# s0 = deltax
# s1 = deltay
# s2 = st
# s3 = error
# s4 = y
# s5 = ystep
# s6 = x

for1:	beq	$s6, $t7, end4	# end for loop if equal (x0 to x1 inclusive)

	addi	$t0, $0, 1	# $t0 = 1
	bne	$s2, $t0, else4	# if $s2 == 1 (if st == 1) 
	sw	$s4, 0($s7)	# plotting y
	sw	$s6, 4($s7)	# plotting x
	addi	$s7, $s7, 8	# incrementing data mem loc
	j	yx		# don't fall through

else4:	sw	$s6, 0($s7)	# plotting x
	sw	$s4, 4($s7)	# plotting y
	addi	$s7, $s7, 8	# incrementing data mem loc

yx:	add	$s3, $s3, $s1	# error = error + deltay
	
	sll	$t1, $s3, 1	# $t1 = 2*error
	addi	$t2, $s0, -1	# $t2 = deltax - 1
	slt	$t3, $t2, $t1	# is $t2 < $t1 (deltax - 1 < 2*error)
	beq	$t3, $0, else5	# branch if deltax - 1 > 2*error
	add	$s4, $s4, $s5	# y = y + ystep
	sub	$s3, $s3, $s0	# error = error - deltax

else5:	addi	$s6, $s6, 1	# increment $s6 (x) for for loop
	j for1			# for loop

end4:	jr $ra

end:	add	$0, $0, $0
	add	$0, $0, $0
