#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define FREE 0
#define ALLOCATED 1

struct Metadata
{
    size_t startingOffset;
    size_t size;
    int allocationId;
    int status;
};

size_t poolSize, blockCapacity = 4, blockCount = 1;
int allocStrategy, allocId = 1;
unsigned char *pool = NULL;
struct Metadata *block = NULL;

int readInteger(char *prompt, int *value)
{
    char line[128];
    int index;

    while (1)
    {
        char *newLine = NULL;

        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("Inavlid input! enter valid integer: \n");
            return 0;
        }

        for (index = 0; line[index] != '\0'; index++)
        {
            if (line[index] == '\n')
            {
                newLine = &line[index];
                break;
            }
        }
        int tooLong = 0;
        if (newLine == NULL && !feof(stdin))
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                tooLong = 1;
            }

            if (tooLong)
            {
                printf("too long input!\n");
                continue;
            }
        }

        errno = 0;
        char *endptr;

        long number = strtol(line, &endptr, 10);

        if (endptr == line)
        {
            printf("Invalid Input! enter an integer\n");
            continue;
        }
        while (isspace((unsigned char)*endptr))
        {
            endptr++;
        }
        if (*endptr != '\0')
        {
            printf("Invalid Input! enter an integer\n");
            continue;
        }

        if (errno == ERANGE || number < INT_MIN || number > INT_MAX)
        {
            printf("Number is outside valid int range!\n");
            continue;
        }

        *value = (int)number;

        return 1;
    }
}

int allocationStrategy()
{
    while (1)
    {
        if (!readInteger("Select allocation strategy (1. First fit, 2. Best fit, 3. Worst fit): ", &allocStrategy))
            return 0;
        if (allocStrategy < 1 || allocStrategy > 3)
        {
            printf("Invalid allocation strategy!");
            continue;
        }
        break;
    }
    switch (allocStrategy)
    {
    case 1:
        printf("Strategy set to First Fit.\n");
        return 1;
    case 2:
        printf("Strategy set to Best Fit.\n");
        return 1;
    case 3:
        printf("Strategy set to Worst Fit.\n");
        return 1;
    }
    return 0;
}

int initializePool()
{
    int tempPoolSize;
    while (1)
    {
        if (!readInteger("Enter memory pool size in bytes: ", &tempPoolSize))
            return 0;
        if (tempPoolSize < 1 || tempPoolSize > 1000000)
        {
            printf("Invalid pool size, must be in between 1 to 1000000!\n");
            continue;
        }
        break;
    }

    poolSize = (size_t)tempPoolSize;

    pool = malloc(poolSize);
    if (pool == NULL)
    {
        printf("Memory pool initialization failed!\n");
        return 0;
    }

    if (!allocationStrategy())
    {
        free(pool);
        pool = NULL;
        return 0;
    }

    block = malloc(blockCapacity * sizeof *block);
    if (block == NULL)
    {
        free(pool);
        pool = NULL;
        printf("Block-Metadata initialization failed!\n");
        return 0;
    }
    block[0].allocationId = 0;
    block[0].size = poolSize;
    block[0].startingOffset = 0;
    block[0].status = FREE;

    blockCount = 1;
    printf("Memory pool initialized successfully!\n");
    return 1;
}

void displayMap()
{
    printf("Memory map (%zu bytes):\n", poolSize);

    for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        block[blockIndex].status == FREE ? printf("[ Free: ") : printf("[ Allocated: #%d: ", block[blockIndex].allocationId);
        printf("%zu bytes, ", block[blockIndex].size);
        printf("Start: %zu ]\n", block[blockIndex].startingOffset);
    }
}

void displayStatistics()
{
    printf("Total Memory: %zu bytes\n", poolSize);

    size_t allocatedBytes = 0, freeBytes = 0, largestFree = 0;
    int freeBlock = 0, allocBlock = 0;

    for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        if (block[blockIndex].status == ALLOCATED)
        {
            allocatedBytes += block[blockIndex].size;
            allocBlock++;
        }
        else if (block[blockIndex].status == FREE)
        {
            freeBytes += block[blockIndex].size;
            freeBlock++;

            if (block[blockIndex].size > largestFree)
                largestFree = block[blockIndex].size;
        }
    }

    printf("Allocated: %zu bytes\n", allocatedBytes);
    printf("Allocated blocks: %d\n", allocBlock);
    printf("Free: %zu bytes\n", freeBytes);
    printf("Free blocks: %d\n", freeBlock);
    printf("Largest free block: %zu bytes\n", largestFree);
    if (freeBlock > 1)
        printf("External Fragmentation: Yes\n");
    else
        printf("External Fragmentation: No\n");
}

void shiftAfterRelease(size_t releaseIndex)
{
    for (size_t blockIndex = releaseIndex; blockIndex < blockCount - 1; blockIndex++)
    {
        block[blockIndex] = block[blockIndex + 1];
    }
    blockCount--;
}

void mergeFreeBlocks(int freeIndex)
{
    if (freeIndex > 0 && block[freeIndex - 1].status == FREE)
    {
        block[freeIndex - 1].size += block[freeIndex].size;

        shiftAfterRelease(freeIndex);
        freeIndex--;
    }
    if (freeIndex + 1 < blockCount && block[freeIndex + 1].status == FREE)
    {
        block[freeIndex].size += block[freeIndex + 1].size;

        shiftAfterRelease(freeIndex + 1);
    }
}

void releaseAllocation()
{
    int releaseId;
    int releaseIndex = -1;
    if (!readInteger("Enter Allocation ID: ", &releaseId))
    {
        printf("Invalid Input! \n");
        return;
    }

    for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        if (block[blockIndex].allocationId == releaseId && block[blockIndex].status == ALLOCATED)
        {
            releaseIndex = (int)blockIndex;
            break;
        }
    }

    if (releaseIndex == -1)
    {
        printf("Invalid Allocation ID!\n");
        return;
    }

    block[releaseIndex].status = FREE;
    block[releaseIndex].allocationId = 0;

    mergeFreeBlocks(releaseIndex);

    printf("Allocation Released!\n");
    return;
}

int resizeMetadata()
{
    size_t newCapacity = blockCapacity * 2;

    struct Metadata *temp = realloc(block, newCapacity * sizeof *block);

    if (temp == NULL)
    {
        printf("Unable to expand metadata!\n");
        return 0;
    }
    block = temp;
    blockCapacity = newCapacity;

    return 1;
}

int allocateAtIndex(int index, size_t requestedSize, int allocationId)
{
    size_t oldOffset = block[index].startingOffset;
    size_t oldSize = block[index].size;

    if (oldSize > requestedSize)
    {
        if (blockCount == blockCapacity)
        {
            if (!resizeMetadata())
                return 0;
        }

        for (size_t blockIndex = blockCount; blockIndex > (size_t)index; blockIndex--)
        {
            block[blockIndex] = block[blockIndex - 1];
        }

        block[index].size = requestedSize;
        block[index].status = ALLOCATED;
        block[index].allocationId = allocationId;

        block[index + 1].startingOffset = oldOffset + requestedSize;
        block[index + 1].size = oldSize - requestedSize;
        block[index + 1].status = FREE;
        block[index + 1].allocationId = 0;

        blockCount++;
    }
    else
    {
        block[index].status = ALLOCATED;
        block[index].allocationId = allocationId;
    }

    return 1;
}

int firstFit(size_t requestedSize)
{
    for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        if (block[blockIndex].status == FREE && block[blockIndex].size >= requestedSize)
            return (int)blockIndex;
    }
    return -1;
}

int bestFit(size_t requestedSize)
{
    size_t difference = poolSize;
    int resultIndex = -1;
    for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        if (block[blockIndex].status == FREE && block[blockIndex].size >= requestedSize)
        {
            size_t currentDifference = block[blockIndex].size - requestedSize;
            if (currentDifference < difference)
            {
                difference = currentDifference;
                resultIndex = (int)blockIndex;
            }
        }
    }
    return resultIndex;
}

int worstFit(size_t requestedSize)
{
    size_t difference = 0;
    int resultIndex = -1;
    for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        if (block[blockIndex].status == FREE && block[blockIndex].size >= requestedSize)
        {
            size_t currentDifference = block[blockIndex].size - requestedSize;
            if (resultIndex == -1 || currentDifference > difference)
            {
                difference = currentDifference;
                resultIndex = (int)blockIndex;
            }
        }
    }
    return resultIndex;
}

void allocateMemory()
{
    int requestSize;
    while (1)
    {
        if (!readInteger("Requested Size: ", &requestSize))
        {
            printf("Invalid Input!");
            return ;
        }

        if (requestSize <= 0 || (size_t)requestSize > poolSize)
        {
            printf("Inavlid Size! Enter in range 1 to %zu\n", poolSize);
            continue;
        }

        break;
    }
    int resultIndex = -1;
    switch (allocStrategy)
    {
    case 1:
        resultIndex = firstFit((size_t)requestSize);
        break;
    case 2:
        resultIndex = bestFit((size_t)requestSize);
        break;
    case 3:
        resultIndex = worstFit((size_t)requestSize);
        break;
    }

    if (resultIndex == -1)
    {
        size_t freeBytes = 0, largestFree = 0;
        for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
        {

            if (block[blockIndex].status == FREE)
            {
                freeBytes += block[blockIndex].size;

                if (block[blockIndex].size > largestFree)
                    largestFree = block[blockIndex].size;
            }
        }
        if (freeBytes >= (size_t)requestSize && largestFree < (size_t)requestSize)
            printf("The request for %zu bytes failed even though %zu bytes are free, because the free memory is fragmented (largest free size is %zu bytes).\n", (size_t)requestSize, freeBytes, largestFree);
        else
            printf("Space not available!\n");

        return ;
    }

    if (!allocateAtIndex(resultIndex, (size_t)requestSize, allocId))
    {
        printf("Memory allocation failed!\n");
        return ;
    }

    printf("Allocation successful! Id: %d, Start: %zu, Size: %zu\n", block[resultIndex].allocationId, block[resultIndex].startingOffset, block[resultIndex].size);
    allocId++;
    return ;
}

void resizeAllocation()
{
    int resizeId;
    int resizeIndex = -1;
    if (!readInteger("Enter Allocation ID: ", &resizeId))
    {
        printf("Invalid Input! \n");
        return;
    }

    for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        if (block[blockIndex].allocationId == resizeId && block[blockIndex].status == ALLOCATED)
        {
            resizeIndex = (int)blockIndex;
            break;
        }
    }

    if (resizeIndex == -1)
    {
        printf("Invalid Allocation ID!\n");
        return;
    }

    int requestSize;
    while (1)
    {
        if (!readInteger("Requested Size: ", &requestSize))
        {
            printf("Invalid Input! \n");
            return;
        }

        if (requestSize <= 0 || (size_t)requestSize > poolSize)
        {
            printf("Invalid Size! Enter in range 1 to %zu\n", poolSize);
            continue;
        }
        break;
    }

    size_t oldSize = block[resizeIndex].size;
    size_t oldOffset = block[resizeIndex].startingOffset;

    if ((size_t)requestSize == oldSize)
    {
        printf("Allocation size is already the same!\n");
        return;
    }

    if ((size_t)requestSize < oldSize)
    {
        size_t releasedSize = oldSize - (size_t)requestSize;

        if (resizeIndex + 1 < blockCount && block[resizeIndex + 1].status == FREE)
        {
            block[resizeIndex + 1].size += releasedSize;
            block[resizeIndex].size = (size_t)requestSize;
            block[resizeIndex + 1].startingOffset = oldOffset + (size_t)requestSize;
        }
        else
        {
            if (blockCount == blockCapacity)
            {
                if (!resizeMetadata())
                {
                    printf("Metadata expansion failed!\n");
                    return;
                }
            }

            for (size_t i = blockCount; i > (size_t)resizeIndex; i--)
            {
                block[i] = block[i - 1];
            }

            block[resizeIndex].size = (size_t)requestSize;
            block[resizeIndex + 1].allocationId = 0;
            block[resizeIndex + 1].startingOffset = block[resizeIndex].startingOffset + (size_t)requestSize;
            block[resizeIndex + 1].size = releasedSize;
            block[resizeIndex + 1].status = FREE;

            blockCount++;
        }
    }
    else if ((size_t)requestSize > oldSize) 
    {
        size_t extraSize = (size_t)requestSize - oldSize;
        if (resizeIndex + 1 < blockCount && block[resizeIndex + 1].status == FREE && block[resizeIndex + 1].size >= extraSize)
        {
            if (block[resizeIndex + 1].size > extraSize)
            {
                block[resizeIndex].size = (size_t)requestSize;

                block[resizeIndex + 1].startingOffset = block[resizeIndex].startingOffset + block[resizeIndex].size;
                block[resizeIndex + 1].size -= extraSize;
            }
            else
            {
                block[resizeIndex].size = (size_t)requestSize;
                shiftAfterRelease(resizeIndex + 1);
            }
        }
        else
        {
            int destinationIndex = -1;
            switch (allocStrategy)
            {
            case 1:
                destinationIndex = firstFit((size_t)requestSize);
                break;
            case 2:
                destinationIndex = bestFit((size_t)requestSize);
                break;
            case 3:
                destinationIndex = worstFit((size_t)requestSize);
                break;
            }
            if (destinationIndex == -1)
            {
                printf("Resizing failed! No enough space available.\n");
                return;
            }
            if (!allocateAtIndex(destinationIndex, (size_t)requestSize, resizeId))
            {
                printf("Unable to allocate new region!\n");
                return;
            }

            size_t newOffset = block[destinationIndex].startingOffset;
            memcpy(pool + newOffset, pool + oldOffset, oldSize);

            int oldIndex = -1;

            for (size_t blockIndex = 0; blockIndex < blockCount; blockIndex++)
            {
                if (block[blockIndex].allocationId == resizeId &&
                    block[blockIndex].status == ALLOCATED &&
                    block[blockIndex].startingOffset == oldOffset)
                {
                    oldIndex = (int)blockIndex;
                    break;
                }
            }
            if (oldIndex != -1)
            {
                block[oldIndex].status = FREE;
                block[oldIndex].allocationId = 0;

                mergeFreeBlocks(oldIndex);
            }
            else{
                printf("Unexpectedly: old allocation not found!\n");
                return;
            }
        }
    }
    printf("Allocation ID %d resized successfully to %d bytes!\n", resizeId, requestSize);

       return;
}

int main()
{
    if (!initializePool())
    {
        printf("Pool initialization failed!\n");
        return 1;
    }
        
    int choice = 0;

    do
    {
        printf("\n1. Allocate memory\n2. Release memory\n3. Resize an allocation\n4. Display memory map\n");
        printf("5. Display memory statistics\n6. Change allocation strategy\n7. Exit\n");

        if (!readInteger("Enter your Choice: ", &choice))
        {
            printf("Invalid Choice!\n");
            break;
        }
            
        if (choice < 1 || choice > 7)
        {
            printf("enter choice in range 1 to 7: \n");
            continue;
        }

        switch (choice)
        {
            case 1:   allocateMemory();     break;

            case 2:  releaseAllocation();   break;

            case 3: resizeAllocation();  break;

            case 4:  displayMap();  break;

            case 5:  displayStatistics();   break;

            case 6:  allocationStrategy();  break;

            case 7:  break;
        }

    } while (choice != 7);

    free(pool);
    free(block);
    printf("Pool And Metadata freed successfully!\n");
    return 0;
}