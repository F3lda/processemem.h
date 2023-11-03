#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>

#include "../../src/processes.h"



//System V Semaphores (wait for zero)

#define SEM_OP_INC 1
#define SEM_OP_DEC -1
#define SEM_OP_WAIT 0
#define SEM_OP_LOCK_OR_WAIT -1
#define SEM_OP_UNLOCK 1



key_t KEY;



union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};



int sem_systemv_named_create(key_t sem_key, int value)
{
    int semID = semget(sem_key, 1, IPC_CREAT | 0666); // 1 = number of semaphores
    if (semID == -1) {
        return -1;
    }
    union semun initValue;
    initValue.val = value; // set value
    if(semctl(semID, 0, SETVAL, initValue) == -1){// semaphore id, semaphore number in array [from 0], command, semun union
        return -2;
    }
    return semID; // pointer or -1, -2
}

int sem_systemv_named_get(key_t sem_key)
{
    return semget(sem_key, 1, IPC_CREAT | 0666); // pointer or -1
}

int sem_systemv_named_destroy(key_t sem_key)
{
    int semID = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semID == -1) {
        return -1;
    }
    return semctl(semID, 1, IPC_RMID, NULL); // 0 - OK, -1 - failure
}

int sem_systemv_named_get_value(int semID)// returns -1 on error
{
	return semctl(semID, 0, GETVAL, 0);// semaphore id, semaphore number in array [from 0], cmd, [ignored]
}

int sem_systemv_named_push_operation(int semID, int operation)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;// number of semaphore in the semaphore array [from 0]
	sem_buf.sem_op = operation;// operation - 1: add 1 to semaphore value (unlock); -1: subtract 1 from semaphore value (lock or wait); 0: wait until semaphore value is zero 
	sem_buf.sem_flg = SEM_UNDO;// flag - 0: wait until operation is done; IPC_NOWAIT: don't wait; SEM_UNDO - automatically undo when the process terminates
    return semop(semID, &sem_buf, 1);// semaphore id, semaphore buffer, length of sem_buf array - RETURN: if successful returns 0, otherwise returns -1
}


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


