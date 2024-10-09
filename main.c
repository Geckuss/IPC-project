#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define BUFFER_SIZE 4

void init_process(int pipes[][2]);
void scheduler(int sync_pipe);          // Pass the sync pipe to scheduler. Scheduler is independent process (Criteria 2)
void wait_for_scheduler(int sync_pipe); // Wait for the scheduler signal in init

int main()
{
    int pipes[BUFFER_SIZE][2];
    int sync_pipe[2]; // Pipe for synchronization between scheduler and init
    pid_t scheduler_pid, init_pid;

    // Create pipes for communication between init and P1-P4
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("Pipe creation failed");
            exit(1);
        }
    }

    // Create synchronization pipe between scheduler and init
    if (pipe(sync_pipe) == -1)
    {
        perror("Sync pipe creation failed");
        exit(1);
    }

    // Fork the scheduler process
    scheduler_pid = fork();
    if (scheduler_pid == 0)
    {
        // Child process: Scheduler
        close(sync_pipe[0]);     // Close reading end of sync pipe in scheduler
        scheduler(sync_pipe[1]); // Pass the writing end of the sync pipe to scheduler
        exit(0);
    }
    else if (scheduler_pid > 0)
    {
        // Fork the init process
        init_pid = fork();
        if (init_pid == 0)
        {
            // Child process: Init
            close(sync_pipe[1]);              // Close writing end of sync pipe in init
            wait_for_scheduler(sync_pipe[0]); // Wait for the scheduler signal
            init_process(pipes);              // Proceed with init process after synchronization
            exit(0);
        }
        else if (init_pid > 0)
        {
            // Parent process: Wait for init and scheduler to finish
            waitpid(init_pid, NULL, 0);      // Wait for init
            waitpid(scheduler_pid, NULL, 0); // Wait for scheduler
        }
        else
        {
            perror("Fork failed for init");
            exit(1);
        }
    }
    else
    {
        perror("Fork failed for scheduler");
        exit(1);
    }

    return 0;
}
