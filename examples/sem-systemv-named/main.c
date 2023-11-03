#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>

#include "../../src/processemem.h"

// SYSTEM V Named Semaphores



key_t KEY;



int process1(int processID, void *data)
{
    int semID = sem_systemv_named_get(KEY);
    if (semID == -1) {
        printf("open SEM failed!\n");
    }

    sem_systemv_named_push_operation(semID, SEM_OP_LOCK_OR_WAIT);

    for (int i=0; i<10; i++) {
        printf("A: %d\n", i);
        usleep(500);
    }

    sem_systemv_named_push_operation(semID, SEM_OP_UNLOCK);

    printf("A1: end\n");
	return 0;
}

int process2(int processID, void *data)
{
    int semID = sem_systemv_named_get(KEY);
    if (semID == -1) {
        printf("open SEM failed!\n");
    }

    for (int i=0; i<10; i++) {
        printf("B: %d\n", i);
        
        if (i == 5) {
            sem_systemv_named_push_operation(semID, SEM_OP_UNLOCK);
        }
        usleep(500);
    }

    sem_systemv_named_push_operation(semID, SEM_OP_LOCK_OR_WAIT);


    printf("B2: end\n");
	return 0;
}

int main()
{
    // generate key
    char* path = getenv("HOME");
    KEY = ftok(path, getpid());


    int semID = sem_systemv_named_create(KEY, 0); // 0 = locked; >0 = only X process are allowed at a time
    if (semID < 0) {
        printf("create SEM failed!\n");
        return 1;
    }


    printf("SEM VALUE: %d\n", sem_systemv_named_get_value(semID));


    ProcessCreate(process1, NULL, false);
    ProcessCreate(process2, NULL, false);
	ProcessWaitForAllChildProcesses();


    printf("SEM VALUE: %d\n", sem_systemv_named_get_value(semID));


    sem_systemv_named_destroy(KEY);

    return 0;
}
