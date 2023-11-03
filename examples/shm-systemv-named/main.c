#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>

#include "../../src/processemem.h"
//#include "../../src/processes.h"
//#include "../../src/sharedmem.h"

// POSIX Named SharedMemory



#define shmSIZE sizeof(int)
key_t KEY;



int process1(int processID, void *data)
{
    int *shm = (int*)shm_systemv_named_open(KEY, shmSIZE);
    if (shm == ((void *) -1)) {
        printf("PROCESS 1 - OPEN SHM failed!\n");
        return -1;
    }

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        printf("A: %d\n", *shm);
        usleep(500);
    }

    shm_systemv_named_close(shm);
	return 0;
}

int process2(int processID, void *data)
{
    int *shm = (int*)shm_systemv_named_open(KEY, shmSIZE);
    if (shm == ((void *) -1)) {
        printf("PROCESS 2 - OPEN SHM failed!\n");
        return -1;
    }

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        printf("B: %d\n", *shm);
        usleep(500);
    }

    shm_systemv_named_close(shm);
	return 0;
}

int main()
{
    // generate key
    char* path = getenv("HOME");
    KEY = ftok(path, getpid());


    int *shm = NULL;
    shm = shm_systemv_named_create(KEY, shmSIZE);
    if (shm == ((void *) -1)) {
        printf("create SHM failed!\n");
        return 1;
    }

    *shm = 20;
    printf("START VALUE: %d\n", *shm);


    ProcessCreate(process1, NULL, false);
    ProcessCreate(process2, NULL, false);
	ProcessWaitForAllChildProcesses();


    printf("END VALUE: %d\n", *shm);


    shm_systemv_named_close(shm);
    shm_systemv_named_destroy(KEY, shmSIZE);

    return 0;
}
