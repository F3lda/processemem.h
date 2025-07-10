/**
 * @file semaphores.c
 * 
 * @brief Functions for working with semaphores
 * @date 2023-11-03
 * @author F3lda
 */
#include "semaphores.h"





/* SEMAPHORES POSIX UNNAMED - (wait if zero) */
/* ----------------------------------------- */
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





/* SEMAPHORES POSIX NAMED - (wait if zero) */
/* --------------------------------------- */
sem_t * sem_posix_named_create(const char *sem_name, unsigned int value)
{
    return sem_open(sem_name, O_CREAT, 0666, value); // pointer or SEM_FAILED
}

sem_t * sem_posix_named_open(const char *sem_name)
{
    return sem_open(sem_name, O_RDWR); // pointer or SEM_FAILED
}

int sem_posix_named_close(sem_t *semaphore)
{
    return sem_close(semaphore);
}

int sem_posix_named_destroy(const char *sem_name)
{
    return sem_unlink(sem_name);
}



/* SEMAPHORES POSIX FUNCS */
/* ---------------------- */
int sem_posix_get_value(sem_t *semaphore)
{
    int value = 0;
    if (sem_getvalue(semaphore, &value) == -1) {
        return -1;
    }
    return value;
}

int sem_posix_lock_or_wait(sem_t *semaphore)
{
    return sem_wait(semaphore);
}

int sem_posix_unlock(sem_t *semaphore)
{
    return sem_post(semaphore);
}





/* SEMAPHORES SYSTEMV NAMED - (wait for zero) */
/* ------------------------------------------ */
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
    sem_buf.sem_flg = 0;// flag - 0: wait until operation is done (semaphore is available); IPC_NOWAIT: don't wait; SEM_UNDO - automatically undo when the process terminates
    return semop(semID, &sem_buf, 1);// semaphore id, semaphore buffer, length of sem_buf array - RETURN: if successful returns 0, otherwise returns -1
}
