#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define BUFFER_SIZE 4

void init_process(int pipes[][2]); // Function prototype
void scheduler();                  // Function prototype

int main()
{
    int pipes[BUFFER_SIZE][2];
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

    // Fork the scheduler process
    scheduler_pid = fork();
    if (scheduler_pid == 0)
    {
        // Child process: Scheduler
        scheduler();
        exit(0);
    }
    else if (scheduler_pid > 0)
    {
        // Fork the init process
        init_pid = fork();
        if (init_pid == 0)
        {
            // Child process: Init
            init_process(pipes);
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
