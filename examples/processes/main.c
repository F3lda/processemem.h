#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../../src/processes.h"



int process1(int processID, void *data)
{
    printf("PROCESS 1 [%d] - STARTED\n", processID);
    sleep(3); // wait 3 seconds
    printf("PROCESS 1 [%d] - DONE\n", processID);
	return 0;
}

int process2(int processID, void *data)
{
    int data_in = *((int *)data);
    int data_out = data_in + 2;

    printf("PROCESS 2 [%d] - STARTED\n", processID);

    printf("data_in: %d\n", data_in);
    printf("data_out: %d\n", data_out);

    printf("PROCESS 2 [%d] - DONE\n", processID);
	return data_out;
}

int main()
{
    printf("main - STARTED\n");
    int data_in = 5;
    printf("PROCESS 1 returned: %d\n", ProcessCreate(process1, NULL, false));
    printf("PROCESS 2 returned: %d\n", ProcessCreate(process2, &data_in, true));
	ProcessWaitForAllChildProcesses();
	printf("main - DONE\n");
    
    return 0;
}
