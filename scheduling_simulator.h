#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H

#include <stdio.h>
#include <ucontext.h>

#include "task.h"

enum TASK_STATE {
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_TERMINATED
};

struct TASK {
    int PID;
    char Task_name[16];
    enum TASK_STATE Task_state;
    int Queueing_time;
    int Time_quantum;
    char Priority;
    char Time;
    int suspend_time;
    void (*task)(void);
    ucontext_t uc;
    void *stack;
};

void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);

int add(char **command);
#endif
