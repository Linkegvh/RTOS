#My first time experimenting with a scheduler of a real time operating system

## More info can be found here about what I am doing: https://drive.google.com/file/d/1GoT179c_I9kwsoZROQ_T0nVnKkPVdWtX/view?usp=sharing

There are a few files here, but they are basically all doing the same thing exact that one is doing it in a more complex scheduler way than the previous:

the list of complexity is rank below:

	normal_blocking_function.c --> this one is purely the most normal way of programming	
	
	basic_function_pointers_to_start_a_task.c --> this one is using functions to start the tasks instead of calling it in the main c
	
	halt_me_and_sleep_function.c --> this one is adding the idea of function halting itself and putting itself to sleep for a few rounds to the scheduler idea
	
	using_structures.c --> this one is the most advance as this one is all about using structures in conjunction with the above few scheduler ways to make the code more expandable to accomodate for future functions adding.