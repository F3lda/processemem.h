#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>

#include "../../src/processes.h"


#define shmSIZE sizeof(int)
key_t KEY;




void * shm_systemv_named_create(key_t shm_key, size_t shm_size)
{
    int shmID = shmget(shm_key, shm_size, IPC_CREAT | 0666);
    if (shmID == -1) {
        return ((void *) -1);
    }
    return shmat(shmID, NULL, 0); // pointer or ((void *) -1)
}

void * shm_systemv_named_open(key_t shm_key, size_t shm_size)
{
    return shm_systemv_named_create(shm_key, shm_size); // pointer or ((void *) -1)
}

int shm_systemv_named_close(void * shm)
{
    return shmdt(shm); // 0 - OK, -1 - failure
}

int shm_systemv_named_destroy(key_t shm_key, size_t shm_size)
{    
    int shmID = shmget(shm_key, shm_size, IPC_CREAT | 0666);
    if (shmID == -1) {
        return -1;
    }
    return shmctl(shmID, IPC_RMID, NULL); // 0 - OK, -1 - failure
}


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
