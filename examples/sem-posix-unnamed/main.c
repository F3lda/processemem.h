#define _GNU_SOURCE // for MAP_ANONYMOUS
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "../../src/processes.h"


// POSIX Semaphores (wait if zero)


void * sem_posix_unnamed_create(unsigned int value)
{
    void *sem;
    sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == ((void *) -1)) {
        return ((void *) -1);
    }
    if (sem_init(sem, 1, value) == -1) {
        return ((void *) -1);
    }
    return sem; // pointer or ((void *) -1)
}

int sem_posix_unnamed_destroy(void * sem)
{
    sem_destroy(sem);
    return munmap(sem, sizeof(sem_t)); // 0 - OK, -1 - failure
}



int sem_posix_unnamed_get_value(sem_t *semaphore)
{
    int value = 0;
	if (sem_getvalue(semaphore, &value) == -1) {
        return -1;
    }
    return value;
}

int sem_posix_unnamed_lock_or_wait(sem_t *semaphore)
{
	return sem_wait(semaphore);
}

int sem_posix_unnamed_unlock(sem_t *semaphore)
{
	return sem_post(semaphore);
}



int process1(int processID, void *data)
{
    sem_t *sem = (sem_t*)data;

    sem_posix_unnamed_lock_or_wait(sem);

    for (int i=0; i<10; i++) {
        printf("A: %d\n", i);
        usleep(500);
    }

    sem_posix_unnamed_unlock(sem);

    printf("A1: end\n");
	return 0;
}

int process2(int processID, void *data)
{
    sem_t *sem = (sem_t*)data;

    for (int i=0; i<10; i++) {
        printf("B: %d\n", i);
        
        if (i == 5) {
            sem_posix_unnamed_unlock(sem);
        }
        usleep(500);
    }

    sem_posix_unnamed_lock_or_wait(sem);
    
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


    printf("SEM VALUE: %d\n", sem_posix_unnamed_get_value(sem));


    ProcessCreate(process1, sem, false);
    ProcessCreate(process2, sem, false);
	ProcessWaitForAllChildProcesses();


    printf("SEM VALUE: %d\n", sem_posix_unnamed_get_value(sem));


    sem_posix_unnamed_destroy(sem);

    return 0;
}
