#include "stdio.h"

extern int asm_streaming(int x, int mean, int reset, int* buf);

// [T]EE2028 Assignment 1, Sem 1, AY2019/20
// (c) CK Tham, ECE NUS

int main(void)
{
	int i,j;

	// Variable definitions
	int x[12]={2,4,2,4,2,4,10,10,10,10,10,2};
	int buffer[5]={0,0,0,0,0};

	// Call asm function with mean 3
	for (i=0;i<12;i++)
	{
		if (i == 0 || i == 11)
			printf("variance: %d ", asm_streaming(x[i],3,1,buffer));
		else
			printf("variance: %d ", asm_streaming(x[i],3,0,buffer));
		printf("buffer: ");
		for (j=0;j<5;j++)
			printf("%d ", buffer[j]);
		printf("\n");
	}

// Enter an infinite loop, just incrementing a counter
	volatile static int loop = 0;
	while (1) {
		loop++;
	}
}

/* Required results from completed asm_streaming():
variance: 1 buffer: 2 0 0 0 0
variance: 1 buffer: 2 4 0 0 0
variance: 1 buffer: 2 4 2 0 0
variance: 1 buffer: 2 4 2 4 0
variance: 1 buffer: 2 4 2 4 2
variance: 1 buffer: 4 4 2 4 2
variance: 10 buffer: 4 10 2 4 2
variance: 20 buffer: 4 10 10 4 2
variance: 29 buffer: 4 10 10 10 2
variance: 39 buffer: 4 10 10 10 10
variance: 49 buffer: 10 10 10 10 10
variance: 1 buffer: 2 10 10 10 10
*/
