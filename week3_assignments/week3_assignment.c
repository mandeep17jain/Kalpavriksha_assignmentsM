#include <stdio.h>
#include <stdlib.h>

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
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void addEmployee()
{
    int managerId;
    struct employees *manager;

    struct employees *newEmployee;
    newEmployee = malloc(sizeof(struct employees));

    if (newEmployee == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter employee ID: ");
    if(scanf("%d", &(newEmployee->employeeId)) != 1 || newEmployee->employeeId <= 0)
    {
        printf("Enter valid Id!\n");
        clearInputBuffer();
        free(newEmployee);
        return;
    }
    if (findEmployee(newEmployee->employeeId) != NULL)
    {
        printf("Employee ID already exists!\n");
        clearInputBuffer();
        free(newEmployee);
        return;
    }

    printf("Enter Name: ");
    if (scanf(" %49[^\n]", newEmployee->employeeName) != 1)
    {
        printf("Invalid name input!\n");
        clearInputBuffer();
        free(newEmployee);
        return;
    }
    clearInputBuffer();

    printf("Enter Designation: ");
    if (scanf(" %49[^\n]", newEmployee->designation) != 1)
    {
        printf("Invalid designation input!\n");
        clearInputBuffer();
        free(newEmployee);
        return;
    }
    clearInputBuffer();

    newEmployee->reportingManager = NULL;
    newEmployee->firstReport = NULL;
    newEmployee->nextReport = NULL;
    newEmployee->nextEmployee = NULL;

    printf("Enter Reporting Manager ID (0 if none): ");
    if(scanf("%d", &managerId) != 1 || managerId < 0)
    {
        printf("Enter valid manager id!");
        clearInputBuffer();
        free(newEmployee);
        return;
    }
    
    if (managerId != 0)
    {
        if (managerId == newEmployee->employeeId)
        {
            printf("An employee cannot be their own manager!\n");
            free(newEmployee);
            return;
        }
        manager = findEmployee(managerId);
        if(employeeListHead == NULL)
        {
            printf("No any employee in organization, first add atleast one employee!\n");
            free(newEmployee);
            return;
        }
        if (manager == NULL)
        {
            printf("Manager Not found!\n");
            free(newEmployee);
            return;
        }
        newEmployee->reportingManager = manager;
    }

    if (newEmployee->reportingManager != NULL)
    {
        newEmployee->nextReport = newEmployee->reportingManager->firstReport;
        newEmployee->reportingManager->firstReport = newEmployee;
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

    for(int i=0;i<level*4;i++)
        printf(" ");

    printf("%d - %s - %s\n", currentEmployee->employeeId, currentEmployee->employeeName, currentEmployee->designation);

    displayEmployeeHierarchy(currentEmployee->firstReport,id,level+1);
    if(currentEmployee->employeeId != id)
        displayEmployeeHierarchy(currentEmployee->nextReport,id,level);  

}

void SelectEmployeeHierarchy()
{
    int employeeId;

    printf("Enter Employee ID: ");

    if (scanf("%d", &employeeId) != 1 || employeeId <= 0)
    {
        printf("Enter a valid ID!\n");
        clearInputBuffer();
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
    if(currentEmployee == NULL)
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

    printf("Enter Employee ID to remove: ");
    if(scanf("%d", &id) != 1 || id <= 0)
    {
        printf("Enter Valid Id\n");
        clearInputBuffer();
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

int main()
{
    int choice;
    do
    {
        printf("\n================ORGANIZATION MENU=================\n");
        printf("1.Add Employee\n2.Remove Employee\n3.Display Employee Hierarchy\n4.Display Complete Hierarchy\n5.Exit\n");
        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1 || choice < 1 || choice > 5)
        {
            printf("Enter Choice in range 1 to 5!\n");
            clearInputBuffer();
            continue;
        }

        switch (choice)
        {
            case 1: addEmployee(); break;

            case 2: removeEmployee(); break;

            case 3: SelectEmployeeHierarchy();  break;

            case 4:
                    displayCompleteHierarchy(employeeListHead);
                    printf("Total Employees: %d\n",countOrganization(employeeListHead));
                    break;

            case 5:  exit(0);  break;

            default:
                printf("Invalid Choice!\n");
        }

    } while (choice != 5);

    return 0;
}