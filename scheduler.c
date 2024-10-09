#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "sorting.h"

#define SHM_KEY 12345
#define BUFFER_SIZE 4

void scheduler()
{
    int shmid;
    int *shared_memory;

    // Create shared memory segment
    if ((shmid = shmget(SHM_KEY, BUFFER_SIZE * sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    // Attach to the shared memory
    if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (int *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    printf("Scheduler: Waiting for data to be written to shared memory...\n");

    // Sleep to simulate the scheduler waiting for data
    sleep(2); // Ensure init has time to write data

    // Sort the list of priorities
    sort_priorities(shared_memory, BUFFER_SIZE);
    printf("Scheduler: Sorted list of priorities:\n");

    // Print the sorted list one by one
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("Priority %d: %d\n", i + 1, shared_memory[i]);
    }
    printf("Scheduler: Finished printing sorted priorities.\n");

    // Detach shared memory
    shmdt(shared_memory);
    printf("Scheduler: Detached from shared memory...\n");

    // Delete shared memory segment
    shmctl(shmid, IPC_RMID, NULL);
}
