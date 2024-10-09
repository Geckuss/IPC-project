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
    int shmid;
    int *shared_memory;

    sleep(1); // Wait for scheduler to create shared memory

    // Attach to the shared memory created by the scheduler
    if ((shmid = shmget(12345, BUFFER_SIZE * sizeof(int), 0666)) < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (int *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    // Fork child processes P1-P4
    printf("Init: Starting to fork child processes P1 to P4...\n");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if ((pid[i] = fork()) == 0)
        {                       // Child process
            close(pipes[i][0]); // Close reading end of pipe

            // Generate random number between 0-19
            srand(time(NULL) + getpid()); // Seed with time and process ID
            int priority = rand() % 20;

            // Print the generated priority
            printf("P%d: Generated priority = %d\n", i + 1, priority);

            // Send random number to init process via pipe
            write(pipes[i][1], &priority, sizeof(priority));
            close(pipes[i][1]); // Close writing end of pipe
            exit(0);            // Child process exits
        }
    }

    // Parent process (init)
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        close(pipes[i][1]); // Close writing end of pipe in parent

        // Read priority number sent by child processes
        int priority;
        read(pipes[i][0], &priority, sizeof(priority));
        close(pipes[i][0]); // Close reading end of pipe

        // Store the received priority in shared memory
        shared_memory[i] = priority;
        printf("Init: Writing priority %d to shared memory...\n", shared_memory[i]);

        // Wait for child processes to finish
        waitpid(pid[i], &status, 0);
    }

    // Detach shared memory
    shmdt(shared_memory);
}
