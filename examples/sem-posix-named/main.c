#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>

#include "../../src/processemem.h"

// POSIX Named Semaphores



#define semNAME "/ios-proj2"



int process1(int processID, void *data)
{
    sem_t *sem = sem_posix_named_open(semNAME);
    if (sem == SEM_FAILED) {
        printf("open SEM failed!\n");
    }

    sem_posix_lock_or_wait(sem);

    for (int i=0; i<10; i++) {
        printf("A: %d\n", i);
        usleep(500);
    }

    sem_posix_unlock(sem);

    sem_posix_named_close(sem);

    printf("A1: end\n");
    return 0;
}

int process2(int processID, void *data)
{
    sem_t *sem = sem_posix_named_open(semNAME);
    if (sem == SEM_FAILED) {
        printf("open SEM failed!\n");
    }

    for (int i=0; i<10; i++) {
        printf("B: %d\n", i);
        
        if (i == 5) {
            sem_posix_unlock(sem);
        }
        usleep(500);
    }

    sem_posix_lock_or_wait(sem);

    sem_posix_named_close(sem);

    printf("B2: end\n");
    return 0;
}

int main()
{
    sem_t *sem;
    sem = sem_posix_named_create(semNAME, 0);
    if (sem == ((void *) -1)) {
        printf("create SEM failed!\n");
        return 1;
    }


    printf("SEM VALUE: %d\n", sem_posix_get_value(sem));


    ProcessCreate(process1, NULL, false);
    ProcessCreate(process2, NULL, false);
    ProcessWaitForAllChildProcesses();


    printf("SEM VALUE: %d\n", sem_posix_get_value(sem));


    sem_posix_named_close(sem);
    sem_posix_named_destroy(semNAME);

    return 0;
}
