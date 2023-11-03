/**
 * @file sharedmem.h
 * 
 * @brief Functions for working with shared memory
 * @date 2023-11-03
 * @author F3lda
 */
#ifndef SHAREDMEM_H_
#define SHAREDMEM_H_

#define _GNU_SOURCE // for S_IRUSR | S_IWUSR and usleep() and MAP_ANONYMOUS
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>





/* SHAREDMEMORY POSIX UNNAMED */
/* -------------------------- */
void * shm_posix_unnamed_create(size_t shm_size); // pointer or MAP_FAILED ((void *) -1)
int shm_posix_unnamed_destroy(void * shm, size_t shm_size); // 0 - OK, -1 - failure



/* SHAREDMEMORY POSIX NAMED */
/* ------------------------ */
void * shm_posix_named_create(const char *shm_name, size_t shm_size); // pointer or MAP_FAILED ((void *) -1)
void * shm_posix_named_open(const char *shm_name, size_t shm_size); // pointer or MAP_FAILED ((void *) -1)
int shm_posix_named_close(void * shm, size_t shm_size); // 0 - OK, -1 - failure
int shm_posix_named_destroy(const char *shm_name); // 0 - OK, -1 - failure



/* SHAREDMEMORY SYSTEMV NAMED */
/* ------------------------ */
void * shm_systemv_named_create(key_t shm_key, size_t shm_size); // pointer or ((void *) -1)
void * shm_systemv_named_open(key_t shm_key, size_t shm_size); // pointer or ((void *) -1)
int shm_systemv_named_close(void * shm); // 0 - OK, -1 - failure
int shm_systemv_named_destroy(key_t shm_key, size_t shm_size); // 0 - OK, -1 - failure

#endif
