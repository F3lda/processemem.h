#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>

#include "../../src/processemem.h"

// POSIX Unnamed SharedMemory



#define shmSIZE sizeof(int)



int process1(int processID, void *data)
{
    int *shm = (int*)data;
    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        printf("A: %d\n", *shm);
        usleep(500);
    }
	return 0;
}

int process2(int processID, void *data)
{
    int *shm = (int*)data;
    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        printf("B: %d\n", *shm);
        usleep(500);
    }
	return 0;
}

int main()
{
    int *shm = NULL;
    shm = shm_posix_unnamed_create(shmSIZE);
    if (shm == MAP_FAILED) {
        printf("MAP_FAILED failed!\n");
        return 1;
    }

    *shm = 20;
    printf("START VALUE: %d\n", *shm);


    ProcessCreate(process1, shm, false);
    ProcessCreate(process2, shm, false);
	ProcessWaitForAllChildProcesses();


    printf("END VALUE: %d\n", *shm);


    shm_posix_unnamed_destroy(shm, shmSIZE);

    return 0;
}
