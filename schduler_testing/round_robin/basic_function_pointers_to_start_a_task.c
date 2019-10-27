// This is one more step on top of building a normal blocking loop
// this is using function pointers to start a task


#include <stdio.h>

void taskA ();
void taskB ();
void taskC ();
void start_function( void (*function_ptr) () );

int main (void) {
        printf("This is the start of the main function\n");
      	start_function (&taskA);
        start_function (&taskB);
	start_function (&taskC);
        return 0;
}

void start_function( void (*function_ptr) ()){
	function_ptr();
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
