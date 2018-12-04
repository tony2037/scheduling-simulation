#include "scheduling_simulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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

int add(){
    printf("\ncommand: add\n");
    
    return 0;
}

int remove_task(){
    printf("\ncommand: remove\n");

    return 0;
}

int ps(){
    printf("\ncommand: ps\n");

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
	    command = get_input(input);

	    for(size_t i = 0; command[i] != NULL; ++i)
	        printf("command[%d]: %s\n", (int)i, command[i]);
	
	    if(!strcmp(command[0], "q") || !strcmp(command[0], "quit")){
	        break;
	    }
	    else if(!strcmp(command[0], "add")){
	        add();
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
