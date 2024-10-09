#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#define BUFFER_SIZE 4

void init_process(int pipes[][2])
{
    int status;
    pid_t pid[BUFFER_SIZE];
    int generated_priorities[BUFFER_SIZE]; // Local buffer to hold generated priorities
    int shmid;
    int *shared_memory;

    // Fork child processes P1-P4 (Criteria 1)
    printf("Init: Starting to fork child processes P1 to P4...\n");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if ((pid[i] = fork()) == 0) // Use fork (Criteria 1)
        {                           // Child process
            close(pipes[i][0]);     // Close reading end of pipe

            // Generate random number between 0-19 (Criteria 1)
            srand(time(NULL) + getpid()); // Seed with time and process ID
            int priority = rand() % 20;

            printf("P%d: Generated priority = %d\n", i + 1, priority);

            // Send random number to init process via pipe (Criteria 1)
            write(pipes[i][1], &priority, sizeof(priority));
            close(pipes[i][1]); // Close writing end of pipe
            exit(0);            // Child process exits
        }
    }

    // Parent process (init)
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        close(pipes[i][1]); // Close writing end of pipe in parent

        // Read priority number sent by child processes (Criteria 1)
        int priority;
        read(pipes[i][0], &priority, sizeof(priority));
        close(pipes[i][0]); // Close reading end of pipe

        // Store the received priority in local buffer
        generated_priorities[i] = priority;

        // Wait for child processes to finish
        waitpid(pid[i], &status, 0);
    }

    if ((shmid = shmget(12345, (BUFFER_SIZE + 1) * sizeof(int), 0666)) < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    // Attach to the shared memory created by the scheduler using shmat (Criteria 3)
    if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (int *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    // Write the generated numbers to shared memory (Criteria 3)
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("Init: Writing priority %d to shared memory...\n", generated_priorities[i]);
        shared_memory[i + 1] = generated_priorities[i];
    }

    shared_memory[0] = 1; // Set flag to 1 (data is ready)

    // Detach from shared memory (Termination)
    shmdt(shared_memory);
}
