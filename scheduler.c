#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "sorting.h"

#define SHM_KEY 12345
#define BUFFER_SIZE 4

void scheduler(int sync_pipe)
{
    int shmid;
    int *shared_memory;

    // Create shared memory segment with shmget (Criteria 2)
    if ((shmid = shmget(SHM_KEY, (BUFFER_SIZE + 1) * sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    // Attach to the shared memory with shmat (Criteria 2)
    if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (int *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    // Signal the init process that shared memory is created
    char signal = 'S';
    write(sync_pipe, &signal, 1); // Send signal to init
    close(sync_pipe);             // Close the pipe after sending signal

    printf("Scheduler: Waiting for data to be written to shared memory...\n");
    while (shared_memory[0] != 1) // Check if the flag is set
    {
        // Busy-wait until the flag indicates data is ready
    }

    // Sort the list of priorities to ascending order with bubble sort or bogo sort :D (Criteria 2)
    // sort_priorities(shared_memory + 1, BUFFER_SIZE);
    bogo_sort(shared_memory + 1, BUFFER_SIZE);
    printf("Scheduler: Sorted list of priorities:\n");

    // Print the sorted list one by one (Criteria 2)
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("Priority %d: %d\n", i + 1, shared_memory[i + 1]);
    }
    printf("Scheduler: Finished printing sorted priorities.\n");

    // Detach from shared memory (Termination)
    shmdt(shared_memory);
    printf("Scheduler: Detached from shared memory...\n");

    // Delete shared memory segment (Termination)
    shmctl(shmid, IPC_RMID, NULL);
    printf("Scheduler: Shared memory deleted...\n");
}
