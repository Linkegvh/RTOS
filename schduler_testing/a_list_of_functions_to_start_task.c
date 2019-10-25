// This function is implemented to try out how to use list of functions
// to achieve a scheduler effect

#include <stdio.h>

#define Num_task 4 // the 4th one is the NULL statement

void taskA ();
void taskB ();
void taskC ();
void start_function( void (*function_ptr) () );
void scheduler (int *);
void (*readytasks[Num_task]) (void *p);

int main (void) {
        printf("This is the start of the main function\n");
	
	int i, task_index = 0;	
	
	// task table
	readytasks[0] = &taskA;
	readytasks[1] = &taskB;
	readytasks[2] = &taskC;
	readytasks[3] = 0;

	// here is the scheduler delcaration
	
	for (i = 0; i < 10; i++){
		scheduler( &task_index );
	}

        return 0;
}

void scheduler(int *counter){
	
	if (readytasks[*counter] == 0 && *counter != 0){
		*counter = 0;
		printf("we have finished one loop\n");
	}else if (readytasks[*counter] == 0 && *counter == 0){
		printf("we are at the start of another loop\n");	
	}

	start_function( readytasks[*counter]);

	*counter += 1;
	
	return;
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

