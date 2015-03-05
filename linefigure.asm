# sp should be initialized to 8191

# a0 = x0
# a1 = y0
# a2 = x1
# a3 = y1

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
	add	$0, $0, $0
