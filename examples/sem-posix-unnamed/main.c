#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>

#include "../../src/processemem.h"

// POSIX Unnamed Semaphores



int process1(int processID, void *data)
{
    sem_t *sem = (sem_t*)data;

    sem_posix_lock_or_wait(sem);

    for (int i=0; i<10; i++) {
        printf("A: %d\n", i);
        usleep(500);
    }

    sem_posix_unlock(sem);

    printf("A1: end\n");
	return 0;
}

int process2(int processID, void *data)
{
    sem_t *sem = (sem_t*)data;

    for (int i=0; i<10; i++) {
        printf("B: %d\n", i);
        
        if (i == 5) {
            sem_posix_unlock(sem);
        }
        usleep(500);
    }

    sem_posix_lock_or_wait(sem);
    
    printf("B2: end\n");
	return 0;
}

int main()
{
    sem_t *sem;
    sem = sem_posix_unnamed_create(0);
    if (sem == ((void *) -1)) {
        printf("create SEM failed!\n");
        return 1;
    }


    printf("SEM VALUE: %d\n", sem_posix_get_value(sem));


    ProcessCreate(process1, sem, false);
    ProcessCreate(process2, sem, false);
	ProcessWaitForAllChildProcesses();


    printf("SEM VALUE: %d\n", sem_posix_get_value(sem));


    sem_posix_unnamed_destroy(sem);

    return 0;
}
