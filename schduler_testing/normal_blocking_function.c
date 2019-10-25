#include <stdio.h>

void taskA ();
void taskB ();
void taskC ();

int main (void) {
	printf("This is the start of the main function\n");
	taskA();
	taskB();
	taskC();	
	return 0;
}


void taskA (){
	printf("This is task A\n");
	return;
}


void taskB (){
	printf("This is task B\n");
	return;
}

void taskC (){
	printf("This is task C\n");
	return;
}
