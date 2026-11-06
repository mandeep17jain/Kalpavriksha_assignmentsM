#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
int readInteger(const char *prompt, int *value)
{
    char line[128];
    char *endptr;
    long number;

    while (1)
    {
        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            return 0;  
        }

        int tooLong = 0;
        char *newline = NULL;

        for (int lineIndex = 0; line[lineIndex] != '\0'; lineIndex++)
        {
            if (line[lineIndex] == '\n')
            {
                newline = &line[lineIndex];
                break;
            }
        }

        if (newline == NULL && !feof(stdin))
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                tooLong = 1;
            }

            if (tooLong || ch == '\n')
            {
                printf("Input line is too long!\n");
                continue;
            }
        }

        errno = 0;
        number = strtol(line, &endptr, 10);

        if (endptr == line)
        {
            printf("Invalid input! Enter an integer.\n");
            continue;
        }

        while (isspace((unsigned char)*endptr))
        {
            endptr++;
        }

        if (*endptr != '\0')
        {
            printf("Invalid input! Enter only an integer.\n");
            continue;
        }

        if (errno == ERANGE ||
            number < INT_MIN || number > INT_MAX)
        {
            printf("Number is outside the valid int range!\n");
            continue;
        }

        *value = (int)number;
        return 1;
    }
}
int main()
{
    int size, target;
    if(!readInteger("Enter size: ",&size))
    {
        printf("Invalid Input!\n");
        return 1;
    }
    if(size <= 0 || size > 100)
    {
        printf("enter size in range 1 to 100!\n");
        return 0;
    }
    int arr[size];

    printf("Enter array elements: ");
    for (int i = 0; i < size; i++)
    {
        scanf("%d", &arr[i]);
    }

    if(!readInteger("Enter target: ",&target))
    {
        printf("Invalid Input!\n");
        return 1;
    }
    int start = 0, sum = 0, found = 0;

    for (int end = 0; end < size; end++)
    {
        sum += arr[end];

        while (sum > target && start <= end)
        {
            sum -= arr[start];
            start++;
        }

        if (sum == target)
        {
            printf("%d %d", start + 1, end + 1);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Not found!");

    return 0;
}