#include "scheduling_simulator.h"

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
    char *separator = " ";
    char *pared;
    
    parsed = strtok(input, separator);
    size_t i = 0;
    while(pared != NULL){
       command[i++] = parsed;
       parsed = strtok(NULL, separator);
    }

    command[i] = NULL;
    return command;
}

int main()
{
	return 0;
}
