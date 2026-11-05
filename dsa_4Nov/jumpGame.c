#include <stdio.h>

int canJump(int nums[], int n)
{
    int maxReach = 0;

    for (int i = 0; i < n; i++)
    {
        if (i > maxReach)
        {
            return 0;
        }

        int reach = i + nums[i];

        if (reach > maxReach)
        {
            maxReach = reach;
        }

        if (maxReach >= n - 1)
        {
            return 1;
        }
    }

    return 1;
}

int main()
{
    int nums[] = {2, 3, 1, 1, 4};

    int n = sizeof(nums) / sizeof(nums[0]);

    if (canJump(nums, n))
    {
        printf("Last index is reachable.\n");
    }
    else
    {
        printf("Last index is not reachable.\n");
    }

    return 0;
}