
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<errno.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
void generateMatrix(int matrixSize, int (*matrix)[matrixSize])
{
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        int *currentRow = *(matrix + rowIndex);

        for (int columnIndex = 0; columnIndex < matrixSize; columnIndex++)
        {
            *(currentRow + columnIndex) = rand() % 256;
        }
    }
}

void displayMatrix(int matrixSize, int (*matrix)[matrixSize])
{
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        int *currentRow = *(matrix + rowIndex);

        for (int columnIndex = 0; columnIndex < matrixSize; columnIndex++)
        {
            printf("%3d ", *(currentRow + columnIndex));
        }

        printf("\n");
    }
}

void rotateMatrix(int matrixSize, int (*matrix)[matrixSize])
{
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        int *currentRow1 = *(matrix + rowIndex);

        for (int columnIndex = rowIndex + 1; columnIndex < matrixSize; columnIndex++)
        {
            int *currentRow2 = *(matrix + columnIndex);

            int *firstElement = currentRow1 + columnIndex;
            int *secondElement = currentRow2 + rowIndex;

            int temporaryValue = *firstElement;
            *firstElement = *secondElement;
            *secondElement = temporaryValue;
        }
    }

    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        int *currentRow = *(matrix + rowIndex);
        int *leftElement = currentRow;
        int *rightElement = currentRow + matrixSize - 1;

        while (leftElement < rightElement)
        {
            int temporaryValue = *leftElement;
            *leftElement = *rightElement;
            *rightElement = temporaryValue;

            leftElement++;
            rightElement--;
        }
    }
}

void smoothMatrix(int matrixSize, int (*matrix)[matrixSize])
{
    int *previousRow = malloc(matrixSize * sizeof(int));
    int *currentRow = malloc(matrixSize * sizeof(int));
    int *nextRow = malloc(matrixSize * sizeof(int));

    if (previousRow == NULL || currentRow == NULL || nextRow == NULL)
    {
        printf("Memory allocation failed!\n");

        free(previousRow);
        free(currentRow);
        free(nextRow);

        return;
    }

    int *firstRow = *matrix;

    for (int columnIndex = 0; columnIndex < matrixSize; columnIndex++)
    {
        *(currentRow + columnIndex) = *(firstRow + columnIndex);
    }

    int *secondRow = *(matrix + 1);

    for (int columnIndex = 0; columnIndex < matrixSize; columnIndex++)
    {
        *(nextRow + columnIndex) = *(secondRow + columnIndex);
    }

    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        int *outputRow = *(matrix + rowIndex);

        for (int columnIndex = 0; columnIndex < matrixSize; columnIndex++)
        {
            int neighborSum = 0;
            int neighborCount = 0;

            for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
            {
                int neighborRow = rowIndex + rowOffset;

                if (neighborRow < 0 || neighborRow >= matrixSize)
                {
                    continue;
                }

                for (int columnOffset = -1; columnOffset <= 1; columnOffset++)
                {
                    int neighborColumn = columnIndex + columnOffset;

                    if (neighborColumn < 0 || neighborColumn >= matrixSize)
                    {
                        continue;
                    }

                    int neighborValue;

                    if (neighborRow == rowIndex - 1)
                    {
                        neighborValue = *(previousRow + neighborColumn);
                    }
                    else if (neighborRow == rowIndex)
                    {
                        neighborValue = *(currentRow + neighborColumn);
                    }
                    else
                    {
                        neighborValue = *(nextRow + neighborColumn);
                    }

                    neighborSum += neighborValue;
                    neighborCount++;
                }
            }

            *(outputRow + columnIndex) = neighborSum / neighborCount;
        }

        int *temporaryRow = previousRow;
        previousRow = currentRow;
        currentRow = nextRow;
        nextRow = temporaryRow;

        if (rowIndex + 2 < matrixSize)
        {
            int *sourceRow = *(matrix + rowIndex + 2);

            for (int j = 0; j < matrixSize; j++)
            {
                *(nextRow + j) = *(sourceRow + j);
            }
        }
    }

    free(previousRow);
    free(currentRow);
    free(nextRow);
}
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
    int matrixSize=0;

    readInteger("Enter matrix size (2-10): ",&matrixSize);

    if (matrixSize < 2 || matrixSize > 10)
    {
        printf("Invalid matrix size!\n");
        return 1;
    }

    int matrix[matrixSize][matrixSize];

    srand((unsigned int)time(NULL));

    generateMatrix(matrixSize, matrix);

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrixSize, matrix);

    rotateMatrix(matrixSize, matrix);

    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    displayMatrix(matrixSize, matrix);

    smoothMatrix(matrixSize, matrix);

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrixSize, matrix);

    return 0;
}
