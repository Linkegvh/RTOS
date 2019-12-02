.syntax unified
 	.cpu cortex-m3
 	.thumb
 	.align 2

@  [T]EE2028 Assignment 1, Sem 1, AY2019/20
@  (c) CK Tham, ECE NUS

 	.global	asm_streaming
 	.thumb_func

 	.lcomm CBptr 4 @ static pointer pointing to the address of the last executed line
 	.lcomm CBctn 4 @ static number counter that starts from 0 and will get reset after it is number 4
	.lcomm Ncounter 4 @ global counter to count the cycles after reset

asm_streaming:
@	write assembly language instructions here
	// R0 = value, R1 = mean, R2 = reset, R3 = first buffer address, We are not using registers beyond R10
	push {R4 - R10} @ push away the content from register 4 to 10, so that we can use them without changing the original content inside.


// first if - else statement → to reset the program counter as well as address pointer when reset is pressed

	CMP R2, #0 @ comparing the reset with 0 to check if reset is activated
	ITTT NE @ reset activated
	movne R8, #0 @ let register R8 be 0
	ldrne R9, =CBctn @ loading the address of CBctn
	strne R8, [R9]  @ reset the counter to 0

	ITTT NE @ reset activated
	ldrne R10, =Ncounter
	movne R9, #1
	strne R9, [R10] @ reset the Ncounter to value 1

// second if - else statement

	ldr R8, =CBctn
	LDR R4, [R8] @ loading the counter value into R4 for comparison
	CMP R4, #0 @ check if counter is 0
	ITT EQ @ if counter is 0
	ldreq R9, =CBptr
	streq R3, [R9] @ point CBptr to buf0

// end of the if - else statement

	ldr R8, =CBptr @ load the address pointer into R8
	ldr R9, [R8]
	str R0, [R9] @ store the value of x into the address pointer

//here is the subroutine
	// use R4, R5, R6
	// using R4 as the looper counter which will start with 01*04 and every loop will increase with 01*04 till max 20
	// using R5 as the value loader
	// using R6 as the total value before division

	// PUSH {R4-R12} @ store  R4, R5, ….. , R12 to the stack

	mov R4, #4 @ flush the register R4 with 4 before the start of the loop
	mov R6, #0 @ flush the total value with 0 before the start of the loop

	loop:  @ this is the address of where the loop will come back to

	mov R8, R3 @ loading the starting address to R8

	@ this step is to increment the address pointer that we are getting our values from the buffer.
	mov R9, #4
	sub R9, R4, R9
	add R8, R8, R9
	ldr R5, [R8] @ loading the value from memory address into the R5

	// here begins the mathematical calculation

	SUB R7, R5, R1 @ Subtracting Xmean from X
	MUL R8, R7, R7 @ Getting the square of (Xi - X mean)
	ADD R6, R6, R8 @ Total value before division

// here ends the total calculation

// Here starts the breakloop command if we are not going to run this loop 5 times
	ldr R7, =Ncounter
	ldr R8, [R7] @ loading the value of global counter into register 8

    @ this is to find out how many times we have looped
	mov R10,#4
	sdiv R9, R4, R10 @ R9 contains the value of how many times this small loop has looped

	CMP R8, R9 @ compare to check small loop number of times is the same as the number of cycles after reset
	ITT EQ @ if they are the same, which will only happen if the number of cycles after reset is smaller than 5
	sdiveq R0, R6, R8 @ getting the variance right here
	BEQ breakloop @ break away from the small loop

// Here ends the breakloop command and starts the IT block to run the loop once again

	ADD R4, R4, #4 @ increase the loop counter with a number of 4 → meaning will point to the next address for R5

	CMP R4, #24  @ checking if R4 has been looped for 5 times, if it is, we are going to stop looping
	IT NE  @ if now is not at the end of the 4 times loop
	BNE loop @ change the IR to point to the start of the loop again

// division by 5 happens here

	mov R9, #5 @ moving 5 into R8
	SDIV R0, R6, R9 @ Dividing the square of (Xi - X mean) by n (SDIV numerator, denominator)

// end of the division

	breakloop:

	ldr R9, =CBptr
	LDR R8, [R9] @ load the value from address pointer
	ADD R8, R8, #4 @ increment the address by 4 to point to the next address
	STR R8, [R9] @ storing the value from register to the memory

	LDR R9, =CBctn @ load the value from counter
	ldr R8, [R9]
	CMP R8, #4 @ check if counter is 4
	ITE EQ  @ if counter is 4
	moveq R8, #0 @ change the R8 value to 0
	addne R8, R8, #1 @ plus 1 for the counter

	str R8, [R9] @ storing the counter value from R8 back to counter’s location in memory

//this is to increment global counter by 1
	ldr R8, =Ncounter
	ldr R9, [R8]
	add R9, R9, #1
	str R9, [R8] @ increment 1 for the counter in every big cycle

	pop {R4 - R10} @ poping values from R0 into the main C program

 	bx	LR


