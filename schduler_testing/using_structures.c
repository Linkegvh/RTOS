#include <stdio.h>

typedef struct TCBstruct{
	void (*function_ptr) (void *p);
	void *arg_ptr;
	int state;
	int delay;
}tcb;

#define STATE_RUNNING 0
#define STATE_READY 1
#define STATE_WAITING 2
#define STATE_INACTIVE 3

#define N_MAX_TASKS 4

tcb TaskList [N_MAX_TASKS];

void taskA();
void taskB();
void taskC();
void halt_me();
void start_task(int);
void delay(int);
void scheduler();
void start_function(void (*) ());

int C_index = 0;
int N_runB = 0;

int main(){
	int j = 0;
	int task_B_Arg;

	TaskList[j].function_ptr = taskA;
	TaskList[j].arg_ptr = NULL;
	TaskList[j].state = STATE_INACTIVE;
	TaskList[j].delay = -1;
	j++;

	TaskList[j].function_ptr = taskB;
        task_B_Arg = 12;
	int *ip = &task_B_Arg;
	TaskList[j].arg_ptr = (void*) ip;
        TaskList[j].state = STATE_READY;
        TaskList[j].delay = -1;
        j++;

	TaskList[j].function_ptr = taskC;
        TaskList[j].arg_ptr = NULL;
        TaskList[j].state = STATE_READY;
        TaskList[j].delay = -1;
        j++;

	TaskList[j].function_ptr = NULL;
	
	while (N_runB != 5){
		scheduler();
	}

	return 0;
}

void scheduler(){
	if (TaskList[C_index].function_ptr == NULL && C_index != 0){
		C_index = 0;
		printf("End of a loop\n");
	}else if (TaskList[C_index].function_ptr == NULL && C_index == 0){
		printf("We have run all that can be run\n");
	}
	
	printf("this is our current index: %d\n", C_index);
	
	switch (TaskList[C_index].state){
		case 1: start_function(TaskList[C_index].function_ptr); C_index ++; break;
		case 2: if (TaskList[C_index].delay > 0) {
			       TaskList[C_index].delay--;
			}else{ 
				TaskList[C_index].delay = -1;
				start_task(C_index);
			}
			C_index ++; break;
		default: C_index ++;	
	}

	return;
}

void start_function (void (*functionPTR) () ){
	TaskList[C_index].state = STATE_RUNNING;
	functionPTR();
	return;
}

void taskA(){
	printf("This is task A\n");
	halt_me();
	return;
}

void taskB(){
	printf("this is task B");
	int *p = (int *) TaskList[C_index].arg_ptr;
	printf(" with a number: %d\n", *p);
	
	int First_task = 0;
	start_task(First_task);
	
	delay(5);
	N_runB++;
	return;
}

void taskC(){
	printf("this is task C\n");
	TaskList[C_index].state = STATE_READY;
	return;
}

void halt_me(){
	TaskList[C_index].state = STATE_INACTIVE;
	return;
}

void start_task(int task_id){
	TaskList[task_id].state = STATE_READY;
	return;
}
void delay(int d){
	TaskList[C_index].delay = d;
	TaskList[C_index].state = STATE_WAITING;
	return;
}
