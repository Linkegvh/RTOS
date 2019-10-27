// This is one step ahead by incorporating a auto halt and sleep functionality

#include <stdio.h>

#define Num_task 4 // the 4th one is the NULL statement

void taskA ();
void taskB ();
void taskC ();
void start_function( void (*function_ptr) () );
void scheduler ();
void (*readytasks[Num_task]) (void *p);
void (*haltedtask[Num_task]) (void *p);
void (*waitingtask[Num_task]) (void *p);
void halt_me();
void sleep(int);

int halted_index = 0, task_index = 0, waiting_list = 0;
int delay_array[Num_task];

int main (void) {
        printf("This is the start of the main function\n");
	
	int i;	
	
	// task table
	readytasks[0] = &taskA;
	readytasks[1] = &taskB;
	readytasks[2] = &taskC;
	readytasks[3] = 0;

	// here is the scheduler delcaration
	
	for (i = 0; i < 20; i++){
		printf("No %d: \n", i+1);
		scheduler( &task_index );
	}

        return 0;
}

void halt_me(){
	int i;
	haltedtask[halted_index] = readytasks[task_index];
	for (i = task_index; i < Num_task - 1; i++){
		readytasks[i] = readytasks[i+1];
	}

	readytasks[Num_task - 1] = 0;

	halted_index ++;
	task_index --;

	return;
}

void sleep(int time){
	int i;
	waitingtask[waiting_list] = readytasks[task_index];
	for (i = task_index; i < Num_task - 1; i++){
		readytasks[i] = readytasks[i + 1];
	}

	readytasks[Num_task - 1] = 0;
	
	delay_array[waiting_list] = time;

	waiting_list ++;
	task_index --;

	return;
}

void scheduler(){
	int i, j;

	if (readytasks[task_index] == 0 && task_index != 0){
		task_index = 0;
		printf("we have finished one loop\n");
		printf("\n===========================\n");
	}else if (readytasks[task_index] == 0 && task_index == 0){
		printf("we are at the start of another loop\n");	
	}

	start_function( readytasks[task_index]);

	task_index ++;
	
	if (waiting_list != 0){
		for (i = 0; i < waiting_list; i ++){
			if (delay_array[i] == 0){
				readytasks[Num_task - waiting_list - halted_index - 1] = waitingtask[i];
				for (j = 0; j < waiting_list - 1; j++){
					waitingtask[i] = waitingtask[i + 1];
				}
				i --;
				waiting_list --;
			}else {
				delay_array[i] --;
			}
		}
	}

	return;
}

void start_function( void (*function_ptr) ()){
        function_ptr();
}


void taskA (){
        printf("This is task A\n");
        printf("Task A entering halt\n");
	halt_me();
	return;
}


void taskB (){
        printf("This is task B\n");
	printf("Task B entering sleep & will sleep for 5 turns\n");
	sleep(5);
        return;
}

void taskC (){
        printf("This is task C\n");
        return;
}

