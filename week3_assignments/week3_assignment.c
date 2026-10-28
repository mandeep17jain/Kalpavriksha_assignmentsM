#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
struct employees
{
    int employeeId;
    char employeeName[50];
    char designation[50];

    struct employees *reportingManager;
    struct employees *firstReport;
    struct employees *nextReport;
    struct employees *nextEmployee;
};

struct employees *employeeListHead = NULL;

struct employees *findEmployee(int id)
{
    struct employees *currentEmployee = employeeListHead;
    while (currentEmployee != NULL)
    {
        if (currentEmployee->employeeId == id)
            return currentEmployee;

        currentEmployee = currentEmployee->nextEmployee;
    }
    return NULL;
}

void clearInputBuffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
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

        for (int i = 0; line[i] != '\0'; i++)
        {
            if (line[i] == '\n')
            {
                newline = &line[i];
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

void addEmployee()
{
    int managerId, employeeId;
    struct employees *manager = NULL;
    char employeeName[50];
    char designation[50];

    if (!readInteger("Enter employee ID: ", &employeeId))
        return;
    
    if (employeeId <= 0)
    {
        printf("Enter valid ID!\n");
        return;
    }
    if (findEmployee(employeeId) != NULL)
    {
        printf("Employee ID already exists!\n");
        return;
    }

    printf("Enter Name: ");
    if (scanf(" %49[^\n]", employeeName) != 1)
    {
        printf("Invalid name input!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    printf("Enter Designation: ");
    if (scanf(" %49[^\n]", designation) != 1)
    {
        printf("Invalid designation input!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (!readInteger("Enter Reporting Manager ID (0 if none): ", &managerId))
    {
        return;
    }

    if (managerId < 0)
    {
        printf("Enter valid manager ID!\n");
        return;
    }
    if (managerId == employeeId)
    {
        printf("An employee cannot be their own manager!\n");
        return;
    }
    if (managerId != 0)
    {
        manager = findEmployee(managerId);
        if (manager == NULL)
        {
            printf("Manager not found!\n");
            return;
        }
    }
    else
    {
        struct employees *currentEmployee = employeeListHead;
        while (currentEmployee != NULL)
        {
            if (currentEmployee->reportingManager == NULL)
            {
                printf("CEO already exists! Only one CEO is allowed.\n");
                return;
            }
            currentEmployee = currentEmployee->nextEmployee;
        }
    }

    struct employees *newEmployee;
    newEmployee = malloc(sizeof(struct employees));

    if (newEmployee == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    newEmployee->employeeId = employeeId;

    strcpy(newEmployee->employeeName, employeeName);
    strcpy(newEmployee->designation, designation);

    newEmployee->reportingManager = manager;
    newEmployee->firstReport = NULL;
    newEmployee->nextReport = NULL;
    newEmployee->nextEmployee = NULL;

    if (manager != NULL)
    {
        newEmployee->nextReport = manager->firstReport;
        manager->firstReport = newEmployee;
    }

    newEmployee->nextEmployee = employeeListHead;
    employeeListHead = newEmployee;

    printf("Employee added successfully!\n");

}

int countEmployees(struct employees *currentEmployee)
{
    if (currentEmployee == NULL)
        return 0;

    return 1 + countEmployees(currentEmployee->firstReport) + countEmployees(currentEmployee->nextReport);
}

void displayEmployeeHierarchy(struct employees *currentEmployee, int id, int level)
{
    if (currentEmployee == NULL)
        return;

    for (int i = 0; i < level * 4; i++)
        printf(" ");

    printf("%d - %s - %s\n", currentEmployee->employeeId, currentEmployee->employeeName, currentEmployee->designation);

    displayEmployeeHierarchy(currentEmployee->firstReport, id, level + 1);
    if (currentEmployee->employeeId != id)
        displayEmployeeHierarchy(currentEmployee->nextReport, id, level);
}

void SelectEmployeeHierarchy()
{
    int employeeId;

    if (!readInteger("Enter Employee ID: ", &employeeId))
    {
        printf("Invalid Input!\n");
        return;
    }

    if (employeeId <= 0)
    {
        printf("Enter a valid ID!\n");
        return;
    }
    struct employees *employee = findEmployee(employeeId);

    if (employee == NULL)
    {
        printf("Employee not found!\n");
        return;
    }

    displayEmployeeHierarchy(employee, employee->employeeId, 0);

    printf("Total Employees Under %d: %d\n", employee->employeeId, countEmployees(employee->firstReport));
}

int countOrganization(struct employees *currentEmployee)
{
    if (currentEmployee == NULL)
        return 0;

    if (currentEmployee->reportingManager == NULL)
    {
        return countEmployees(currentEmployee) + countOrganization(currentEmployee->nextEmployee);
    }

    return countOrganization(currentEmployee->nextEmployee);
}

void displayCompleteHierarchy(struct employees *currentEmployee)
{
    if (currentEmployee == NULL)
        return;

    if (currentEmployee->reportingManager == NULL)
        displayEmployeeHierarchy(currentEmployee, currentEmployee->employeeId, 0);

    displayCompleteHierarchy(currentEmployee->nextEmployee);
}

int countDirectReports(struct employees *currentEmployee)
{
    int reportCount = 0;
    if (currentEmployee == NULL)
        return 0;

    struct employees *temp = currentEmployee->firstReport;

    while (temp != NULL)
    {
        reportCount++;
        temp = temp->nextReport;
    }

    return reportCount;
}

void removeEmployee()
{

    int id;

    if (!readInteger("Enter Employee ID to remove: ", &id))
    {
        return;
    }

    if (id <= 0)
    {
        printf("Enter a valid ID!\n");
        return;
    }

    struct employees *employeeToRemove = findEmployee(id);

    if (employeeToRemove == NULL)
    {
        printf("Employee not found!\n");
        return;
    }
    if (employeeToRemove->reportingManager == NULL)
    {
        printf("The CEO cannot be removed!\n");
        return;
    }

    struct employees *manager = employeeToRemove->reportingManager;

    struct employees *sibling = NULL;
    struct employees *currentNode = manager->firstReport;

    while (currentNode != NULL)
    {
        if (currentNode != employeeToRemove)
        {
            if (sibling == NULL || countDirectReports(currentNode) < countDirectReports(sibling))
            {
                sibling = currentNode;
            }
        }

        currentNode = currentNode->nextReport;
    }

    struct employees *newManager = sibling;

    if (newManager == NULL)
        newManager = manager;

    currentNode = manager->firstReport;
    struct employees *previousNode = NULL;

    while (currentNode != NULL)
    {
        if (currentNode == employeeToRemove)
        {
            if (previousNode == NULL)
                manager->firstReport = currentNode->nextReport;
            else
                previousNode->nextReport = currentNode->nextReport;

            break;
        }

        previousNode = currentNode;
        currentNode = currentNode->nextReport;
    }

    if (employeeToRemove->firstReport != NULL)
    {
        struct employees *currentReport = employeeToRemove->firstReport;

        while (currentReport != NULL)
        {
            currentReport->reportingManager = newManager;
            currentReport = currentReport->nextReport;
        }

        struct employees *lastReport = employeeToRemove->firstReport;

        while (lastReport->nextReport != NULL)
            lastReport = lastReport->nextReport;

        lastReport->nextReport = newManager->firstReport;
        newManager->firstReport = employeeToRemove->firstReport;

        employeeToRemove->firstReport = NULL;
    }

    currentNode = employeeListHead;
    previousNode = NULL;

    while (currentNode != NULL)
    {
        if (currentNode == employeeToRemove)
        {
            if (previousNode == NULL)
                employeeListHead = currentNode->nextEmployee;
            else
                previousNode->nextEmployee = currentNode->nextEmployee;

            break;
        }

        previousNode = currentNode;
        currentNode = currentNode->nextEmployee;
    }

    free(employeeToRemove);

    printf("Employee removed successfully!\n");
}

void freeEmployeeList()
{
    struct employees *currentEmployee = employeeListHead;
    while (currentEmployee != NULL)
    {
        struct employees *tempNode = currentEmployee;
        currentEmployee = currentEmployee->nextEmployee;
        free(tempNode);
    }
    employeeListHead = NULL;
}

int main()
{
    int choice = 0;
    do
    {
        printf("\n================ORGANIZATION MENU=================\n");
        printf("1.Add Employee\n2.Remove Employee\n3.Display Employee Hierarchy\n4.Display Complete Hierarchy\n5.Exit\n");

        if (!readInteger("Enter your choice: ", &choice))
        {
            printf("\nInput ended. Exiting program.\n");
            break;
        }

        if (choice < 1 || choice > 5)
        {
            printf("Enter Choice in range 1 to 5!\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            addEmployee();
            break;

        case 2:
            removeEmployee();
            break;

        case 3:
            SelectEmployeeHierarchy();
            break;

        case 4:
            displayCompleteHierarchy(employeeListHead);
            printf("Total Employees: %d\n", countOrganization(employeeListHead));
            break;

        case 5:
            break;
        }
    } while (choice != 5);

    freeEmployeeList();
    printf("All employee memory released. Exiting program.\n");

    return 0;
}