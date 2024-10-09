#include "sorting.h"

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
