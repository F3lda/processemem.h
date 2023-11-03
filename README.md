# processemem.h
Basic functions for processes, semaphores and shared memory in unix

## Notes
- Semaphores -> synchronization over multiple processes
- Unnamed semaphore on threads == "mutex"
- Mutex -> synchronization over multiple threads in one process

- ipcs -s               - list semaphores
- ipcrm -s SEMID        - kill semaphore
- ps -fu USERNAME       - list user's processes
- kill -9 PID           - kill process (forced)
- netstat --listen
