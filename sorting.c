#include "sorting.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

// Bubble sort function
void sort_priorities(int *priorities, int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - 1 - i; j++)
        {
            if (priorities[j] > priorities[j + 1])
            {
                int temp = priorities[j];
                priorities[j] = priorities[j + 1];
                priorities[j + 1] = temp;
            }
        }
    }
}

// Bogo sort bonus :D

// Helper function to check if the array is sorted
bool is_sorted(int *priorities, int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        if (priorities[i] > priorities[i + 1])
        {
            return false;
        }
    }
    return true;
}

// Helper function to shuffle the array randomly
void shuffle(int *priorities, int size)
{
    for (int i = 0; i < size; i++)
    {
        int rand_index = rand() % size;
        int temp = priorities[i];
        priorities[i] = priorities[rand_index];
        priorities[rand_index] = temp;
    }
}

// Bogo sort function
void bogo_sort(int *priorities, int size)
{
    srand(time(NULL)); // Seed the random number generator
    int shuffle_count = 0;
    while (!is_sorted(priorities, size))
    {
        shuffle(priorities, size); // Shuffle until sorted
        shuffle_count++;
    }
    printf("Bogo Sort: Array sorted after %d shuffles.\n", shuffle_count);
}
