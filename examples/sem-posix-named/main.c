#define _GNU_SOURCE // for usleep()
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>


#include "../../src/processes.h"


// POSIX Semaphores (wait if zero)


#define semNAME "/ios-proj2"



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



int sem_posix_named_get_value(sem_t *semaphore)
{
    int value = 0;
	if (sem_getvalue(semaphore, &value) == -1) {
        return -1;
    }
    return value;
}

int sem_posix_named_lock_or_wait(sem_t *semaphore)
{
	return sem_wait(semaphore);
}

int sem_posix_named_unlock(sem_t *semaphore)
{
	return sem_post(semaphore);
}



int process1(int processID, void *data)
{
    sem_t *sem = sem_posix_named_open(semNAME);
    if (sem == SEM_FAILED) {
        printf("open SEM failed!\n");
    }

    sem_posix_named_lock_or_wait(sem);

    for (int i=0; i<10; i++) {
        printf("A: %d\n", i);
        usleep(500);
    }

    sem_posix_named_unlock(sem);

    sem_posix_named_close(sem);

    printf("A1: end\n");
	return 0;
}

int process2(int processID, void *data)
{
    sem_t *sem = sem_posix_named_open(semNAME);
    if (sem == SEM_FAILED) {
        printf("open SEM failed!\n");
    }

    for (int i=0; i<10; i++) {
        printf("B: %d\n", i);
        
        if (i == 5) {
            sem_posix_named_unlock(sem);
        }
        usleep(500);
    }

    sem_posix_named_lock_or_wait(sem);

    sem_posix_named_close(sem);

    printf("B2: end\n");
	return 0;
}

int main()
{
    sem_t *sem;
    sem = sem_posix_named_create(semNAME, 0);
    if (sem == ((void *) -1)) {
        printf("create SEM failed!\n");
        return 1;
    }


    printf("SEM VALUE: %d\n", sem_posix_named_get_value(sem));


    ProcessCreate(process1, NULL, false);
    ProcessCreate(process2, NULL, false);
	ProcessWaitForAllChildProcesses();


    printf("SEM VALUE: %d\n", sem_posix_named_get_value(sem));


    sem_posix_named_close(sem);
    sem_posix_named_destroy(semNAME);

    return 0;
}






/*


void proc1() {
    sem_t *sem;

    sem = sem_open(semNAME, O_RDWR);

    sem_wait(sem);
    for (int i=0; i<10; i++) {
        fprintf(stdout, "first: %d\n", i);
        usleep(500);
    }
    fprintf(stdout, "first: end\n");

    sem_close(sem);
}

void proc2() {
    sem_t *sem;

    sem = sem_open(semNAME, O_RDWR);

    for (int i=0; i<10; i++) {
        fprintf(stdout, "second: %d\n", i);
        // po splneni podminky dovolime dalsimu procesu pokracovat
        if (i==5) sem_post(sem);
        usleep(500);
    }
    fprintf(stdout, "second: end\n");

    sem_close(sem);
}

//int main(int argc, char *argv[])
int main(void)
{
    int pid;
    sem_t *sem;
    pid_t consPID;
    pid_t prodPID;
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);

    // vytvorime a inicializujeme semafory
    sem = sem_open(semNAME, O_CREAT, 0666, 0);
    sem_close(sem);

    // systemove volani - vzdy je vhodne overit uspesnost!
    if ((pid = fork()) < 0) {
        perror("fork"); 
        exit(2);
    }

    if (pid == 0) { // child
        proc1();
        exit(0);
    } else { // parent.
        consPID = pid;
        //--
        pid = fork();
        //--
        if (pid == 0) { // child
            proc2();
            exit(0);
        } else { // parent
            prodPID = pid;
        }
    }

    // pockame az vsichni skonci
    waitpid(consPID, NULL, 0);
    waitpid(prodPID, NULL, 0);

    // zrusime zdroje
    sem_unlink(semNAME);

    return 0;
}
*/