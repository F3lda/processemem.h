/**
 * @file sharedmem.c
 * 
 * @brief Functions for working with shared memory
 * @date 2023-11-03
 * @author F3lda
 */
#include "sharedmem.h"





/* SHAREDMEMORY POSIX UNNAMED */
/* -------------------------- */
void * shm_posix_unnamed_create(size_t shm_size)
{
    return (void *)mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // pointer or MAP_FAILED ((void *) -1)
}

int shm_posix_unnamed_destroy(void * shm, size_t shm_size)
{
    return munmap(shm, shm_size); // 0 - OK, -1 - failure
}





/* SHAREDMEMORY POSIX NAMED */
/* ------------------------ */
void * shm_posix_named_create(const char *shm_name, size_t shm_size)
{
    int shmID = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmID == -1) {
        return ((void *) -1);
    }
    if (ftruncate(shmID, shm_size) == -1) {
        close(shmID);
        shm_unlink(shm_name);
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
    return shm_unlink(shm_name); // 0 - OK, -1 - failure
}





/* SHAREDMEMORY SYSTEMV NAMED */
/* ------------------------ */
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
