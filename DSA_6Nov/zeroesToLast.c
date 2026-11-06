#include <stdio.h>
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
    int size;
    if (!readInteger("Enter Size: ", &size))
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

    int position = 0;

    for (int i = 0; i < size; i++)
    {
        if (arr[i] != 0)
        {
            arr[position] = arr[i];
            position++;
        }
    }

    while (position < size)
    {
        arr[position] = 0;
        position++;
    }

    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }

    return 0;
}