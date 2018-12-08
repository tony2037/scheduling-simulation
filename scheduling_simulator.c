#include "scheduling_simulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>


struct TASK highQueue[50] = {0};
struct TASK lowQueue[50] = {0};
int distribute_pid = 0;
size_t highP_n = 0;
size_t lowP_n = 0;
size_t terminate_n = 0;


void hw_suspend(int msec_10)
{
	return;
}

void hw_wakeup_pid(int pid)
{
	return;
}

int hw_wakeup_taskname(char *task_name)
{
    return 0;
}

int hw_task_create(char *task_name)
{
    return 0; // the pid of created task name
}

char **get_input(char *input){
    char **command = malloc(8* sizeof(char *));
    char *separator = " \n";
    char *parsed;
    
    parsed = strtok(input, separator);
    size_t i = 0;
    while(parsed != NULL){
       command[i++] = parsed;
       parsed = strtok(NULL, separator);
    }

    command[i] = NULL;
    return command;
}

void terminate(){
    printf("terminate\n");
}

int add(char **command){
    printf("\ncommand: add\n");
  
    char tq = 'S';
    char pr = 'L';
    
    for(size_t i = 0; command[i] != NULL; i++){
        if(!strcmp(command[i], "-t")){
	    if(!strcmp(command[i + 1], "L")){
	        tq = 'L';
	    }
	}

        if(!strcmp(command[i], "-p")){
	    if(!strcmp(command[i + 1], "H")){
	        pr = 'H';
	    }
	}
    }

    // parse TASK_NAME
    if(!strcmp(command[1], "task1")){
        ++distribute_pid;

	if(pr == 'H'){
	    //highQueue[highP_n] = (struct TASK) malloc(sizeof(struct TASK));
            highQueue[highP_n].PID = distribute_pid;
	    strcpy(highQueue[highP_n].Task_name, "task1");
            highQueue[highP_n].Task_state = TASK_READY;
            highQueue[highP_n].Queueing_time = 0;
	    if(tq == 'S')
                highQueue[highP_n].Time_quantum = 10;
	    else
                highQueue[highP_n].Time_quantum = 20;
            highQueue[highP_n].Priority = 'H';
            highQueue[highP_n].Time = tq;
            highQueue[highP_n].suspend_time = 0;
            highQueue[highP_n].task = task1;
	    getcontext(&(highQueue[highP_n].uc));
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, terminate, 0);

	    ++highP_n;
	}
	else{
	    //lowQueue[lowP_n] = (struct TASK)malloc(sizeof(struct TASK));
            lowQueue[lowP_n].PID = distribute_pid;
	    strcpy(lowQueue[lowP_n].Task_name, "task1");
            lowQueue[lowP_n].Task_state = TASK_READY;
            lowQueue[lowP_n].Queueing_time = 0;
	    if(tq == 'S')
                lowQueue[lowP_n].Time_quantum = 10;
	    else
                lowQueue[lowP_n].Time_quantum = 20;
            lowQueue[lowP_n].Priority = 'L';
            lowQueue[lowP_n].Time = tq;
            lowQueue[lowP_n].suspend_time = 0;
            lowQueue[lowP_n].task = task1;
	    getcontext(&(lowQueue[lowP_n].uc));
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, terminate, 0);

	    ++lowP_n;
	}
    }
    else if(!strcmp(command[1], "task2")){
        ++distribute_pid;

	if(pr == 'H'){
	    //highQueue[highP_n] = (struct TASK) malloc(sizeof(struct TASK));
            highQueue[highP_n].PID = distribute_pid;
	    strcpy(highQueue[highP_n].Task_name, "task2");
            highQueue[highP_n].Task_state = TASK_READY;
            highQueue[highP_n].Queueing_time = 0;
	    if(tq == 'S')
                highQueue[highP_n].Time_quantum = 10;
	    else
                highQueue[highP_n].Time_quantum = 20;
            highQueue[highP_n].Priority = 'H';
            highQueue[highP_n].Time = tq;
            highQueue[highP_n].suspend_time = 0;
            highQueue[highP_n].task = task2;
	    getcontext(&(highQueue[highP_n].uc));
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, terminate, 0);

	    ++highP_n;
	}
	else{
	    //lowQueue[lowP_n] = (struct TASK)malloc(sizeof(struct TASK));
            lowQueue[lowP_n].PID = distribute_pid;
	    strcpy(lowQueue[lowP_n].Task_name, "task2");
            lowQueue[lowP_n].Task_state = TASK_READY;
            lowQueue[lowP_n].Queueing_time = 0;
	    if(tq == 'S')
                lowQueue[lowP_n].Time_quantum = 10;
	    else
                lowQueue[lowP_n].Time_quantum = 20;
            lowQueue[lowP_n].Priority = 'L';
            lowQueue[lowP_n].Time = tq;
            lowQueue[lowP_n].suspend_time = 0;
            lowQueue[lowP_n].task = task2;
	    getcontext(&(lowQueue[lowP_n].uc));
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, terminate, 0);

	    ++lowP_n;
	}
    }
    else if(!strcmp(command[1], "task3")){
        ++distribute_pid;

	if(pr == 'H'){
	    //highQueue[highP_n] = (struct TASK) malloc(sizeof(struct TASK));
            highQueue[highP_n].PID = distribute_pid;
	    strcpy(highQueue[highP_n].Task_name, "task3");
            highQueue[highP_n].Task_state = TASK_READY;
            highQueue[highP_n].Queueing_time = 0;
	    if(tq == 'S')
                highQueue[highP_n].Time_quantum = 10;
	    else
                highQueue[highP_n].Time_quantum = 20;
            highQueue[highP_n].Priority = 'H';
            highQueue[highP_n].Time = tq;
            highQueue[highP_n].suspend_time = 0;
            highQueue[highP_n].task = task3;
	    getcontext(&(highQueue[highP_n].uc));
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, terminate, 0);

	    ++highP_n;
	}
	else{
	    //lowQueue[lowP_n] = (struct TASK)malloc(sizeof(struct TASK));
            lowQueue[lowP_n].PID = distribute_pid;
	    strcpy(lowQueue[lowP_n].Task_name, "task3");
            lowQueue[lowP_n].Task_state = TASK_READY;
            lowQueue[lowP_n].Queueing_time = 0;
	    if(tq == 'S')
                lowQueue[lowP_n].Time_quantum = 10;
	    else
                lowQueue[lowP_n].Time_quantum = 20;
            lowQueue[lowP_n].Priority = 'L';
            lowQueue[lowP_n].Time = tq;
            lowQueue[lowP_n].suspend_time = 0;
            lowQueue[lowP_n].task = task3;
	    getcontext(&(lowQueue[lowP_n].uc));
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, terminate, 0);

	    ++lowP_n;
	}
    }
    else if(!strcmp(command[1], "task4")){
        ++distribute_pid;

	if(pr == 'H'){
	    //highQueue[highP_n] = (struct TASK) malloc(sizeof(struct TASK));
            highQueue[highP_n].PID = distribute_pid;
	    strcpy(highQueue[highP_n].Task_name, "task4");
            highQueue[highP_n].Task_state = TASK_READY;
            highQueue[highP_n].Queueing_time = 0;
	    if(tq == 'S')
                highQueue[highP_n].Time_quantum = 10;
	    else
                highQueue[highP_n].Time_quantum = 20;
            highQueue[highP_n].Priority = 'H';
            highQueue[highP_n].Time = tq;
            highQueue[highP_n].suspend_time = 0;
            highQueue[highP_n].task = task4;
	    getcontext(&(highQueue[highP_n].uc));
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, terminate, 0);

	    ++highP_n;
	}
	else{
	    //lowQueue[lowP_n] = (struct TASK)malloc(sizeof(struct TASK));
            lowQueue[lowP_n].PID = distribute_pid;
	    strcpy(lowQueue[lowP_n].Task_name, "task4");
            lowQueue[lowP_n].Task_state = TASK_READY;
            lowQueue[lowP_n].Queueing_time = 0;
	    if(tq == 'S')
                lowQueue[lowP_n].Time_quantum = 10;
	    else
                lowQueue[lowP_n].Time_quantum = 20;
            lowQueue[lowP_n].Priority = 'L';
            lowQueue[lowP_n].Time = tq;
            lowQueue[lowP_n].suspend_time = 0;
            lowQueue[lowP_n].task = task4;
	    getcontext(&(lowQueue[lowP_n].uc));
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, terminate, 0);

	    ++lowP_n;
	}
    }
    else if(!strcmp(command[1], "task5")){
        ++distribute_pid;

	if(pr == 'H'){
	    //highQueue[highP_n] = (struct TASK) malloc(sizeof(struct TASK));
            highQueue[highP_n].PID = distribute_pid;
	    strcpy(highQueue[highP_n].Task_name, "task5");
            highQueue[highP_n].Task_state = TASK_READY;
            highQueue[highP_n].Queueing_time = 0;
	    if(tq == 'S')
                highQueue[highP_n].Time_quantum = 10;
	    else
                highQueue[highP_n].Time_quantum = 20;
            highQueue[highP_n].Priority = 'H';
            highQueue[highP_n].Time = tq;
            highQueue[highP_n].suspend_time = 0;
            highQueue[highP_n].task = task5;
	    getcontext(&(highQueue[highP_n].uc));
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, terminate, 0);

	    ++highP_n;
	}
	else{
	    //lowQueue[lowP_n] = (struct TASK)malloc(sizeof(struct TASK));
            lowQueue[lowP_n].PID = distribute_pid;
	    strcpy(lowQueue[lowP_n].Task_name, "task5");
            lowQueue[lowP_n].Task_state = TASK_READY;
            lowQueue[lowP_n].Queueing_time = 0;
	    if(tq == 'S')
                lowQueue[lowP_n].Time_quantum = 10;
	    else
                lowQueue[lowP_n].Time_quantum = 20;
            lowQueue[lowP_n].Priority = 'L';
            lowQueue[lowP_n].Time = tq;
            lowQueue[lowP_n].suspend_time = 0;
            lowQueue[lowP_n].task = task5;
	    getcontext(&(lowQueue[lowP_n].uc));
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, terminate, 0);

	    ++lowP_n;
	}
    }
    else if(!strcmp(command[1], "task6")){
        ++distribute_pid;

	if(pr == 'H'){
	    //highQueue[highP_n] = (struct TASK) malloc(sizeof(struct TASK));
            highQueue[highP_n].PID = distribute_pid;
	    strcpy(highQueue[highP_n].Task_name, "task6");
            highQueue[highP_n].Task_state = TASK_READY;
            highQueue[highP_n].Queueing_time = 0;
	    if(tq == 'S')
                highQueue[highP_n].Time_quantum = 10;
	    else
                highQueue[highP_n].Time_quantum = 20;
            highQueue[highP_n].Priority = 'H';
            highQueue[highP_n].Time = tq;
            highQueue[highP_n].suspend_time = 0;
            highQueue[highP_n].task = task6;
	    getcontext(&(highQueue[highP_n].uc));
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, terminate, 0);

	    ++highP_n;
	}
	else{
	    //lowQueue[lowP_n] = (struct TASK)malloc(sizeof(struct TASK));
            lowQueue[lowP_n].PID = distribute_pid;
	    strcpy(lowQueue[lowP_n].Task_name, "task6");
            lowQueue[lowP_n].Task_state = TASK_READY;
            lowQueue[lowP_n].Queueing_time = 0;
	    if(tq == 'S')
                lowQueue[lowP_n].Time_quantum = 10;
	    else
                lowQueue[lowP_n].Time_quantum = 20;
            lowQueue[lowP_n].Priority = 'L';
            lowQueue[lowP_n].Time = tq;
            lowQueue[lowP_n].suspend_time = 0;
            lowQueue[lowP_n].task = task6;
	    getcontext(&(lowQueue[lowP_n].uc));
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, terminate, 0);

	    ++lowP_n;
	}
    }
    else{
        printf("No such task\n");
    }
    
    return 0;
}

int remove_task(){
    printf("\ncommand: remove\n");

    return 0;
}

int ps(){
    printf("\ncommand: ps\n");

    printf("High Priority Queue:\n===========\n");
    for(size_t i = 0; i < highP_n; i++){
        printf("%d    %s    ", highQueue[i].PID, highQueue[i].Task_name);
	switch(highQueue[i].Task_state){
		case TASK_RUNNING:
		       printf("TASK_RUNNING    ");
		       break;
		case TASK_WAITING:
		       printf("TASK_WAITING    ");
		       break;
		case TASK_READY:
		       printf("TASK_READY    ");
		       break;
		case TASK_TERMINATED:
		       printf("TASK_TERMINATED    ");
		       break;
		default:
		       printf("TASK STATE ERROR    ");
		       break;
	}
        printf("%d    ", highQueue[i].Queueing_time);
        printf("%c    %c\n", highQueue[i].Priority, highQueue[i].Time);	
    }
    printf("Low Priority Queue:\n===========\n");
    for(size_t i = 0; i < lowP_n; i++){
        printf("%d    %s    ", lowQueue[i].PID, lowQueue[i].Task_name);
	switch(lowQueue[i].Task_state){
		case TASK_RUNNING:
		       printf("TASK_RUNNING    ");
		       break;
		case TASK_WAITING:
		       printf("TASK_WAITING    ");
		       break;
		case TASK_READY:
		       printf("TASK_READY    ");
		       break;
		case TASK_TERMINATED:
		       printf("TASK_TERMINATED    ");
		       break;
		default:
		       printf("TASK STATE ERROR    ");
		       break;
	}
        printf("%d    ", lowQueue[i].Queueing_time);
        printf("%c    %c\n", lowQueue[i].Priority, lowQueue[i].Time);	
    }
    return 0;
}

int start(){
    printf("\ncommand: start\n");

    return 0;
}

int shell(){
	char **command;
	char *input;
	size_t input_size = 128;
	size_t nchar = 0;
	while(1){
	    input = (char *)malloc(input_size* sizeof(char));
	    memset(input, 0, input_size);
	    printf(">>");
	    if(-1 == (nchar = getline(&input, &input_size, stdin)))
                printf("shell command error \n");
	    if(!strcmp(input, "\n"))
                continue;
	    command = get_input(input);

	    for(size_t i = 0; command[i] != NULL; ++i)
	        printf("command[%d]: %s\n", (int)i, command[i]);
	
	    if(!strcmp(command[0], "q") || !strcmp(command[0], "quit")){
	        break;
	    }
	    else if(!strcmp(command[0], "add")){
	        add(command);
	    }
	    else if(!strcmp(command[0], "remove")){
                remove_task();
	    }
	    else if(!strcmp(command[0], "ps")){
	        ps();
	    }
	    else if(!strcmp(command[0], "start")){
	        start();
	    }
	    else{
	        printf("No Such command\n");
	    }
	}

	return 0;
}

int main()
{
    shell();	
    return 0;
}
