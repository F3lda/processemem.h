#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>



pthread_t tid[2];
pthread_mutex_t lock;



void * thread(void *arg)
{
    int threadId = *((int *)arg);
    printf("Thread %d - started\n", threadId);


    pthread_mutex_lock(&lock);
    printf("Job %d - started\n", threadId);

    sleep(3);

    printf("Job %d - done\n", threadId);
    pthread_mutex_unlock(&lock);
    

    printf("Thread %d - done\n", threadId);
    return NULL;
}

int main(void)
{
    // create mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex init has failed\n");
        return 1;
    }


    // create threads
    int i = 0;
    while (i < sizeof(tid)/sizeof(pthread_t)) {
        int error = pthread_create(&(tid[i++]), NULL, &thread, &i);
        if (error != 0) {
            printf("Thread can't be created: [%s]\n", strerror(error));
        }
    }


    // wait for threads
    i = 0;
    while (i < sizeof(tid)/sizeof(pthread_t)) {
        pthread_join(tid[i++], NULL);
    }


    // destroy mutex
    pthread_mutex_destroy(&lock);


    printf("ALL DONE!\n");
    return 0;
} 
