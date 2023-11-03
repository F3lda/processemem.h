#define _GNU_SOURCE // for MAP_ANONYMOUS
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "../../src/processes.h"



#define shmSIZE sizeof(int)



void * shm_posix_unnamed_create(size_t shm_size)
{
    return (void *)mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // pointer or MAP_FAILED ((void *) -1)
}

int shm_posix_unnamed_destroy(void * shm, size_t shm_size)
{
    return munmap(shm, shm_size); // 0 - OK, -1 - failure
}



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
