#include "scheduling_simulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <limits.h>


struct TASK highQueue[50] = {0};
struct TASK lowQueue[50] = {0};
int distribute_pid = 0;
size_t highP_n = 0;
size_t lowP_n = 0;
size_t terminate_n = 0;
int is_sim = 0;

static ucontext_t uc_shell;
static void *stack_shell;
static ucontext_t uc_simulation;
static void *stack_simulation;
static ucontext_t uc_scheduler;
static void *stack_scheduler;
static ucontext_t uc_terminate;
static void *stack_terminate;


void hw_suspend(int msec_10)
{
    if(terminate_n < highP_n){
       // it is running high priority now
       if(highQueue[0].Task_state == TASK_RUNNING){
           highQueue[0].Task_state = TASK_WAITING;
	   highQueue[0].suspend_time += msec_10* 10;
	   // push forward
	   struct TASK temp = highQueue[0];
           for(size_t i = 1; i < highP_n; i++){
	       highQueue[i - 1] = highQueue[i];
	   }
	   highQueue[highP_n - 1] = temp;
	   swapcontext(&highQueue[highP_n - 1].uc, &uc_scheduler);
       }
       else{
           printf("algorithm wrong\n");
	   exit(3);
       }
    }
    else{
       // it is running low priority now
       if(lowQueue[0].Task_state == TASK_RUNNING){
           lowQueue[0].Task_state = TASK_WAITING;
	   lowQueue[0].suspend_time += msec_10* 10;
	   // push forward
	   struct TASK temp = lowQueue[0];
           for(size_t i = 1; i < lowP_n; i++){
	       lowQueue[i - 1] = lowQueue[i];
	   }
	   lowQueue[lowP_n - 1] = temp;
	   swapcontext(&lowQueue[lowP_n - 1].uc, &uc_scheduler);
       }
       else{
           printf("algorithm wrong\n");
	   exit(3);
       }
    }
	return;
}

void hw_wakeup_pid(int pid)
{
    for(size_t i = 0; i < highP_n; i++){
        if((highQueue[i].PID == pid) && (highQueue[i].Task_state == TASK_WAITING)){
	    // find it
	    printf("Wake up Task[%d]\n", highQueue[i].PID);
	    highQueue[i].Task_state = TASK_READY;
	    return;
	}
    }

    for(size_t i = 0; i < lowP_n; i++){
        if((lowQueue[i].PID == pid) && (lowQueue[i].Task_state == TASK_WAITING)){
	    // find it
	    printf("Wake up Task[%d]\n", lowQueue[i].PID);
	    lowQueue[i].Task_state = TASK_READY;
	    return;
	}
    }

    printf("Did not find it\n");
    return;
}

int hw_wakeup_taskname(char *task_name)
{
    for(size_t i = 0; i < highP_n; i++){
        if((!strcmp(highQueue[i].Task_name, task_name)) && (highQueue[i].Task_state == TASK_WAITING)){
	    // find it
	    printf("Wake up Task[%d]\n", highQueue[i].PID);
	    highQueue[i].Task_state = TASK_READY;
	    return 0;
	}
    }

    for(size_t i = 0; i < lowP_n; i++){
        if((!strcmp(lowQueue[i].Task_name, task_name)) && (lowQueue[i].Task_state == TASK_WAITING)){
	    // find it
	    printf("Wake up Task[%d]\n", lowQueue[i].PID);
	    lowQueue[i].Task_state = TASK_READY;
	    return 0;
	}
    }

    printf("Did not find it\n");
    return 0;
}

int hw_task_create(char *task_name)
{
    char **command;
    command = (char **) malloc(2* sizeof(char *));
    command[0] = (char *) malloc(8);
    memset(command[0], 0, 8);
    command[1] = (char *) malloc(8);
    memset(command[1], 0, 8);
    strcpy(command[0], "add");
    strcpy(command[1], task_name);
    add(command);
    return distribute_pid; // the pid of created task name
}


void terminate(){
    printf("terminate\n");
    int through = 0;
    //++terminate_n;
    if(terminate_n < highP_n){
       // it is running high priority now
       if(highQueue[0].Task_state == TASK_RUNNING){
           highQueue[0].Task_state = TASK_TERMINATED;
	   // push forward
	   struct TASK temp = highQueue[0];
	   through = temp.Time_quantum;
           for(size_t i = 1; i < highP_n; i++){
	       highQueue[i - 1] = highQueue[i];
	   }
	   highQueue[highP_n - 1] = temp;
       }
       else{
           printf("algorithm wrong\n");
	   exit(3);
       }
    }
    else{
       // it is running low priority now
       if(lowQueue[0].Task_state == TASK_RUNNING){
           lowQueue[0].Task_state = TASK_TERMINATED;
	   // push forward
	   struct TASK temp = lowQueue[0];
	   through = temp.Time_quantum;
           for(size_t i = 1; i < lowP_n; i++){
	       lowQueue[i - 1] = lowQueue[i];
	   }
	   lowQueue[lowP_n - 1] = temp;
       }
       else{
           printf("algorithm wrong\n");
	   exit(3);
       }
    }

    // cut down the waiting time
    for(size_t i = 0; i < highP_n; i++){
        if((highQueue[i].suspend_time -= through) <= 0 && (highQueue[i].Task_state == TASK_WAITING)){
	    highQueue[i].suspend_time = 0;
	    highQueue[i].Task_state = TASK_READY;
	}
    }
    for(size_t i = 0; i < lowP_n; i++){
        if((lowQueue[i].suspend_time -= through) <= 0 && (lowQueue[i].Task_state == TASK_WAITING)){
	    lowQueue[i].suspend_time = 0;
	    lowQueue[i].Task_state = TASK_READY;
	}
    }

    ++terminate_n;
}


void checkTerminate(){
    int terminate = 0;
    for(size_t i = 0; i < highP_n; i++){
        if(highQueue[i].Task_state == TASK_TERMINATED)
            ++terminate;
    }

    for(size_t i = 0; i < lowP_n; i++){
        if(lowQueue[i].Task_state == TASK_TERMINATED)
            ++terminate;
    }

    terminate_n = terminate;
}


void allSuspend(char mode){
    int sp_time = INT_MAX; 
    if(mode == 'H'){
        // All tasks in highQueue are waiting
        for(size_t i = 0; i < highP_n; i++){
	    if((highQueue[i].Task_state == TASK_READY) && (highQueue[i].Task_state == TASK_RUNNING)){
	        printf("ERROR: Not all tasks are waiting\n");
		exit(4);
	    }
	    else if(highQueue[i].Task_state == TASK_WAITING){
	        sp_time = (highQueue[i].suspend_time < sp_time)? highQueue[i].suspend_time: sp_time;
	    }
	}
    }
    else{
	// All tasks in lowQueue are waiting
        for(size_t i = 0; i < lowP_n; i++){
	    if((lowQueue[i].Task_state == TASK_READY) && (lowQueue[i].Task_state == TASK_RUNNING)){
	        printf("ERROR: Not all tasks are waiting\n");
		exit(4);
	    }
	    else if(lowQueue[i].Task_state == TASK_WAITING){
	        sp_time = (lowQueue[i].suspend_time < sp_time)? lowQueue[i].suspend_time: sp_time;
	    }
	}
    }

    // now we have minimum suspend time
    for(size_t i = 0; i < highP_n; i++){
        if((highQueue[i].suspend_time -= sp_time) <= 0 && (highQueue[i].Task_state == TASK_WAITING)){
	    highQueue[i].suspend_time = 0;
	    highQueue[i].Task_state = TASK_READY;
	}
    }
    for(size_t i = 0; i < lowP_n; i++){
        if((lowQueue[i].suspend_time -= sp_time) <= 0 && (lowQueue[i].Task_state == TASK_WAITING)){
	    lowQueue[i].suspend_time = 0;
	    lowQueue[i].Task_state = TASK_READY;
	}
    }
    return;
}


void scheduler(){
    // is simulating
    is_sim = 1;

    // construct scheduler ucontext
    getcontext(&uc_scheduler);
    stack_scheduler = (void *) malloc(8192);
    memset(stack_scheduler, 0, 8192);
    /* When there's no need to schedule, end */
    /* no uc_link */
    uc_scheduler.uc_stack.ss_sp = stack_scheduler;
    uc_scheduler.uc_stack.ss_size = sizeof(stack_scheduler);
    makecontext(&uc_scheduler, scheduler, 0);

    // construct terminate ucontext
    memset(&uc_terminate, 0, sizeof(uc_terminate));
    getcontext(&uc_terminate);
    stack_terminate = (void *) malloc(8192);
    memset(stack_terminate, 0, 8192);
    uc_terminate.uc_link = &uc_scheduler;
    uc_terminate.uc_stack.ss_sp = stack_terminate;
    uc_terminate.uc_stack.ss_size = sizeof(stack_terminate);
    makecontext(&uc_terminate, terminate, 0);

    // Double check terminate_n
    checkTerminate();
    
    // if every tasks are terminated, end
    if(terminate_n >= (highP_n + lowP_n)){
        printf("Every tasks are terminated\n");
	exit(0);
    }

    // if not, schedule it
    if(terminate_n < highP_n){
    // Do high priority queue
        // move forward
	int round = 0;
        while(highQueue[0].Task_state != TASK_READY){
            // push forward
	    struct TASK temp = highQueue[0];
	    for(size_t i = 1; i < highP_n; i++){
	        highQueue[i - 1] = highQueue[i];
	    }
	    highQueue[highP_n - 1] = temp;

	    if((++round) == highP_n){
	        allSuspend('H');
		round = 0;
	    }
	}
	// Do this
	printf("Scheduler: TASK PID:(%d) \n", highQueue[0].PID);
	highQueue[0].Task_state = TASK_RUNNING;
	    
	// set timer
	struct itimerval tick;
	memset(&tick, 0, sizeof(tick));
	tick.it_value.tv_sec = 0; // 0sec
	tick.it_value.tv_usec = highQueue[0].Time_quantum; // ms 
	tick.it_interval.tv_sec = 1;
	tick.it_interval.tv_usec = 0;
	int res_t = setitimer(ITIMER_VIRTUAL, &tick, NULL); // send SIGVTALRM 
	if(res_t == -1){
	    printf("timer error\n");
            exit(2);
	}
	// context switch
        setcontext(&highQueue[0].uc);
        
    }
    else{
    // Do low priority queue
        // move forward
	int round = 0;
        while(lowQueue[0].Task_state != TASK_READY){
            struct TASK temp = lowQueue[0];
	    for(size_t i = 1; i < lowP_n; i++){
	        lowQueue[i - 1] = lowQueue[i];
	    }
	    lowQueue[lowP_n - 1] = temp;
	    
	    if((++round) == lowP_n){
	        allSuspend('L');
		round = 0;
	    }
	}
	// Do this
	printf("Scheduler: TASK PID:(%d) \n", lowQueue[0].PID);
	lowQueue[0].Task_state = TASK_RUNNING;
	    
	// set timer
	struct itimerval tick;
	memset(&tick, 0, sizeof(tick));
	tick.it_value.tv_sec = 0; // 0sec
	tick.it_value.tv_usec = lowQueue[0].Time_quantum; // ms 
	tick.it_interval.tv_sec = 1;
	tick.it_interval.tv_usec = 0;
	int res_t = setitimer(ITIMER_VIRTUAL, &tick, NULL); // send SIGVTALRM 
	if(res_t == -1){
	    printf("timer error\n");
            exit(2);
	}
	// context switch
        setcontext(&lowQueue[0].uc);
    } 

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


int add(char **command){
    printf("\ncommand: add\n");
    if(command[1] == NULL){
        printf("leak of argument\n");
	return -1;
    }
  
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
	    highQueue[highP_n].uc.uc_link = &uc_terminate;
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, task1, 0);

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
	    lowQueue[lowP_n].uc.uc_link = &uc_terminate;
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, task1, 0);

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
	    highQueue[highP_n].uc.uc_link = &uc_terminate;
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, task2, 0);

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
	    lowQueue[lowP_n].uc.uc_link = &uc_terminate;
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, task2, 0);

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
	    highQueue[highP_n].uc.uc_link = &uc_terminate;
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, task3, 0);

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
	    lowQueue[lowP_n].uc.uc_link = &uc_terminate;
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, task3, 0);

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
	    highQueue[highP_n].uc.uc_link = &uc_terminate;
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, task4, 0);

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
	    lowQueue[lowP_n].uc.uc_link = &uc_terminate;
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, task4, 0);

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
	    highQueue[highP_n].uc.uc_link = &uc_terminate;
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, task5, 0);

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
	    lowQueue[lowP_n].uc.uc_link = &uc_terminate;
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, task5, 0);

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
	    highQueue[highP_n].uc.uc_link = &uc_terminate;
            highQueue[highP_n].stack = (void *)malloc(8192);
            highQueue[highP_n].uc.uc_stack.ss_sp = highQueue[highP_n].stack;
            highQueue[highP_n].uc.uc_stack.ss_size = sizeof(highQueue[highP_n].stack);
            makecontext(&highQueue[highP_n].uc, task6, 0);

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
	    lowQueue[lowP_n].uc.uc_link = &uc_terminate;
            lowQueue[lowP_n].stack = (void *)malloc(8192);
            lowQueue[lowP_n].uc.uc_stack.ss_sp = lowQueue[lowP_n].stack;
            lowQueue[lowP_n].uc.uc_stack.ss_size = sizeof(lowQueue[lowP_n].stack);
            makecontext(&lowQueue[lowP_n].uc, task6, 0);

	    ++lowP_n;
	}
    }
    else{
        printf("No such task\n");
    }
    
    return 0;

}

int remove_task(char **command){
    printf("\ncommand: remove\n");
    int pid = atoi(command[1]);
    if(!pid){
        printf("PID ERROR\n");
	return -2;
    }
    // seek in high queue
    for(size_t i = 0; i < highP_n; i++){
        if(highQueue[i].PID == pid){
	    // make every entry forward
	    for(size_t j = i + 1; j < highP_n; j++){
	        highQueue[j - 1] = highQueue[j];
	    }
	    // set the last empty
	    memset(&highQueue[highP_n - 1], 0, sizeof(struct TASK));
	    --highP_n;
	    return 0;
	}
    }
    
    // seek in low queue
    for(size_t i = 0; i < lowP_n; i++){
        if(lowQueue[i].PID == pid){
	    // make every entry forward
	    for(size_t j = i + 1; j < lowP_n; j++){
	        lowQueue[j - 1] = lowQueue[j];
	    }
	    // set the last empty
	    memset(&lowQueue[lowP_n - 1], 0, sizeof(struct TASK));
	    --lowP_n;
	    return 0;
	}
    }
    
    return -1;
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
    printf("\nstart simulation\n");
    sleep(1);
    if(is_sim){
        setcontext(&uc_simulation);
    }
    else{   
        scheduler();
    }
    return 0;
}

int shell(){
	char **command;
	char *input;
	size_t input_size = 128;
	size_t nchar = 0;

	// construct uc_shell
	getcontext(&uc_shell);
	stack_shell = (void *) malloc(8192);
	memset(stack_shell, 0, 8192);
	 /* here dont set up uc_link cause when shell over, everything over*/
	uc_shell.uc_stack.ss_sp = stack_shell;
	uc_shell.uc_stack.ss_size = sizeof(stack_shell);
	makecontext(&uc_shell, (void (*)(void)) shell, 0);

	// construct uc_simulation
	getcontext(&uc_simulation);
	stack_simulation = (void *) malloc(8192);
	memset(stack_simulation, 0, 8192);
	 /* here dont set up uc_simulation cause when shell over, everything over*/
	uc_simulation.uc_stack.ss_sp = stack_simulation;
	uc_simulation.uc_stack.ss_size = sizeof(stack_simulation);

	// taking std input
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
                remove_task(command);
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

void signalHandlerSIGVTALRM(int signum){
    printf("Time's up\n");
    if(terminate_n < highP_n){
       // it is running high priority now
       if(highQueue[0].Task_state == TASK_RUNNING){
	   highQueue[0].Task_state = TASK_READY;

	   // push forward
	   struct TASK temp = highQueue[0];
           for(size_t i = 1; i < highP_n; i++){
	       highQueue[i - 1] = highQueue[i];
	   }
	   highQueue[highP_n - 1] = temp;

           swapcontext(&highQueue[highP_n - 1].uc, &uc_scheduler);
       }
       else{
           printf("algorithm wrong\n");
	   exit(3);
       }
    }
    else{
       // it is running low priority now
       if(lowQueue[0].Task_state == TASK_RUNNING){
	   lowQueue[0].Task_state = TASK_READY;

	   // push forward
	   struct TASK temp = lowQueue[0];
           for(size_t i = 1; i < lowP_n; i++){
	       lowQueue[i - 1] = lowQueue[i];
	   }
	   lowQueue[lowP_n - 1] = temp;

           swapcontext(&lowQueue[lowP_n - 1].uc, &uc_scheduler); 
       }
       else{
           printf("algorithm wrong\n");
	   exit(3);
       }
    }
}

void signalHandlerSIGTSTP(int signum){
    printf("\nctrl + z\n");

    // construct simulation ucontext
    memset(&uc_simulation, 0, sizeof(uc_simulation));
    getcontext(&uc_simulation);
    stack_simulation = (void *) malloc(8192);
    memset(stack_simulation, 0, 8192);
    uc_simulation.uc_link = &uc_terminate;  // This should no happen
    uc_simulation.uc_stack.ss_sp = stack_simulation;
    uc_simulation.uc_stack.ss_size = sizeof(stack_simulation);
    
    swapcontext(&uc_simulation, &uc_shell);
}

int main()
{
    signal(SIGTSTP, signalHandlerSIGTSTP);
    signal(SIGVTALRM, signalHandlerSIGVTALRM);
    shell();	
    return 0;
}
