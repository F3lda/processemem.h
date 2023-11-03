#define _GNU_SOURCE // for S_IRUSR | S_IWUSR and usleep()
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "../../src/processes.h"



#define shmSIZE sizeof(int)
#define shmKEY "/shmKey"



void * shm_posix_named_create(const char *shm_name, size_t shm_size)
{
    int shmID = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmID == -1) {
        return ((void *) -1);
    }
    if (ftruncate(shmID, shm_size) == -1) {
        close(shmID);
        shm_unlink(shmKEY);
        return ((void *) -1);
    }
    void *shm = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    close(shmID);
    return shm; // pointer or MAP_FAILED ((void *) -1)
}

void * shm_posix_named_open(const char *shm_name, size_t shm_size)
{
    int shmID = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (shmID == -1) {
        return ((void *) -1);
    }
    void *shm = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    close(shmID);
    return shm; // pointer or MAP_FAILED ((void *) -1)
}

int shm_posix_named_close(void * shm, size_t shm_size)
{
    return munmap(shm, shm_size); // 0 - OK, -1 - failure
}

int shm_posix_named_destroy(const char *shm_name)
{
    return shm_unlink(shmKEY); // 0 - OK, -1 - failure
}



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
