#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>

#include "../../src/processemem.h"

// POSIX Named SharedMemory



#define shmSIZE sizeof(int)
#define shmKEY "/shmKey"



int process1(int processID, void *data)
{
    int *shm = (int*)shm_posix_named_open(shmKEY, shmSIZE);
    if (shm == ((void *) -1)) {
        printf("PROCESS 1 - OPEN SHM failed!\n");
        return -1;
    }

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        printf("A: %d\n", *shm);
        usleep(500);
    }

    shm_posix_named_close(shm, shmSIZE);
	return 0;
}

int process2(int processID, void *data)
{
    int *shm = (int*)shm_posix_named_open(shmKEY, shmSIZE);
    if (shm == ((void *) -1)) {
        printf("PROCESS 2 - OPEN SHM failed!\n");
        return -1;
    }

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        printf("B: %d\n", *shm);
        usleep(500);
    }

    shm_posix_named_close(shm, shmSIZE);
	return 0;
}

int main()
{
    int *shm = NULL;
    shm = shm_posix_named_create(shmKEY, shmSIZE);
    if (shm == MAP_FAILED) {
        printf("MAP_FAILED failed!\n");
        return 1;
    }

    *shm = 20;
    printf("START VALUE: %d\n", *shm);


    ProcessCreate(process1, NULL, false);
    ProcessCreate(process2, NULL, false);
	ProcessWaitForAllChildProcesses();


    printf("END VALUE: %d\n", *shm);


    shm_posix_named_close(shm, shmSIZE);
    shm_posix_named_destroy(shmKEY);

    return 0;
}
