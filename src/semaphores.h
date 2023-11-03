/**
 * @file semaphores.h
 * 
 * @brief Functions for working with semaphores
 * @date 2023-11-03
 * @author F3lda
 */
#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

#define _GNU_SOURCE // for usleep() and MAP_ANONYMOUS
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>





/* SEMAPHORES POSIX UNNAMED */
/* ------------------------ */
void * sem_posix_unnamed_create(unsigned int value); // pointer or ((void *) -1)
int sem_posix_unnamed_destroy(void * sem); // 0 - OK, -1 - failure



/* SEMAPHORES POSIX NAMED */
/* ---------------------- */
sem_t * sem_posix_named_create(const char *sem_name, unsigned int value); // pointer or SEM_FAILED
sem_t * sem_posix_named_open(const char *sem_name); // pointer or SEM_FAILED
int sem_posix_named_close(sem_t *semaphore);
int sem_posix_named_destroy(const char *sem_name);



/* SEMAPHORES POSIX FUNCS */
/* ---------------------- */
int sem_posix_get_value(sem_t *semaphore);
int sem_posix_lock_or_wait(sem_t *semaphore);
int sem_posix_unlock(sem_t *semaphore);





/* SEMAPHORES SYSTEMV NAMED */
/* ------------------------ */
#define SEM_OP_INC 1
#define SEM_OP_DEC -1
#define SEM_OP_WAIT 0
#define SEM_OP_LOCK_OR_WAIT -1
#define SEM_OP_UNLOCK 1

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};

int sem_systemv_named_create(key_t sem_key, int value); // pointer or -1, -2
int sem_systemv_named_get(key_t sem_key); // pointer or -1
int sem_systemv_named_destroy(key_t sem_key); // 0 - OK, -1 - failure
int sem_systemv_named_get_value(int semID); // returns -1 on error
int sem_systemv_named_push_operation(int semID, int operation); // if successful returns 0, otherwise returns -1

#endif
