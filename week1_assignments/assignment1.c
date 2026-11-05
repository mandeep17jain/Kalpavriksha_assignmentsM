#include <stdio.h>
#define MAX 100

struct student
{
    int rollNumber;
    char name[30];
    int marks1, marks2, marks3;
};
struct student students[MAX];

int findTotal(int index)
{
    return students[index].marks1 + students[index].marks2 + students[index].marks3;
}

float findAverage(int totalMarks)
{
    return totalMarks / 3.0;
}

char findGrade(float averageMarks)
{
    char grade;
    if (averageMarks >= 85)
        grade = 'A';
    else if (averageMarks >= 70)
        grade = 'B';
    else if (averageMarks >= 50)
        grade = 'C';
    else if (averageMarks >= 35)
        grade = 'D';
    else
        grade = 'F';

    return grade;
}

void rollNumbers(int index, int totalStudents)
{
    if (index >= totalStudents)
        return;
    printf("%d ", students[index].rollNumber);
    rollNumbers(index + 1, totalStudents);
}

int main()
{
    int totalStudents, index;
    while (1)
    {
        scanf("%d", &totalStudents);
        if (totalStudents < 1 || totalStudents > 100)
        {
            printf("\nEnter number of students in range 1 to 100!");
            continue;
        }
        else
            break;
    }

    for (index = 0; index < totalStudents; index++)
    {
        scanf("%d", &students[index].rollNumber);
        scanf("%29s", students[index].name);
        while (1)
        {
            scanf("%d %d %d", &students[index].marks1, &students[index].marks2, &students[index].marks3);
            if (students[index].marks1 < 0 || students[index].marks1 > 100 || students[index].marks2 < 0 || students[index].marks2 > 100 || students[index].marks3 < 0 || students[index].marks3 > 100)
            {
                printf("Enter marks in range 0 to 100! Re-enter all 3 marks: \n");
                continue;
            }
            else
                break;
        }
    }

    printf("\nOutput: \n");
    for (index = 0; index < totalStudents; index++)
    {
        printf("\nRoll: %d", students[index].rollNumber);
        printf("\nName: %s", students[index].name);

        int totalMarks = findTotal(index);
        printf("\nTotal: %d", totalMarks);

        float averageMarks = findAverage(totalMarks);
        printf("\nAverage: %.2f", averageMarks);

        char grade = findGrade(averageMarks);
        printf("\nGrade: %c", grade);
        if (averageMarks < 35)
            continue;

        switch (grade)
        {
        case 'A':
            printf("\nPerformance: *****\n");
            break;
        case 'B':
            printf("\nPerformance: ****\n");
            break;
        case 'C':
            printf("\nPerformance: ***\n");
            break;
        case 'D':
            printf("\nPerformance: **\n");
            break;
        }
    }

    printf("\n\nList of Roll numbers (via recursion): ");
    rollNumbers(0, totalStudents);

    return 0;
}