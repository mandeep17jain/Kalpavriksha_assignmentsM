#include <stdio.h>
struct users
{
    int userId;
    char userName[20];
    int userAge;
};
void createFile()
{
    FILE *filePtr;
    filePtr = fopen("users.txt","a");
    if(filePtr == NULL)
    {
        printf("Unable to create File!");
        return;
    }
    printf("File created successfully!");
    fclose(filePtr);
}
void createUser(int userCount)
{
    struct users user1;
    
    user1.userId = userCount;
    printf("Enter user name: ");
    scanf(" %19[^\n]",user1.userName);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF); 
    printf("Enter user Age: ");
    scanf("%d",&user1.userAge);

    FILE *filePtr;
    filePtr = fopen("users.txt","a");
    if(filePtr == NULL)
    {
        printf("\nUnable to open File!");
        return;
    }
    fprintf(filePtr,"%d|%s|%d\n",user1.userId,user1.userName,user1.userAge);
    printf("\nUser created successfully!");
    fclose(filePtr);
}

void displayUsers()
{
    FILE *filePtr;
    filePtr = fopen("users.txt","r");
    if(filePtr == NULL)
    {
        printf("\nUnable to open File!");
        return;
    }
    struct users user1;
    printf("\n----------------------Users Lists-------------------------");
    printf("\nID\tName\t\t\t\tAge");
    printf("\n----------------------------------------------------------");
    while(fscanf(filePtr,"%d|%19[^|]|%d",&user1.userId,user1.userName,&user1.userAge) == 3)
    {
        printf("\n%d",user1.userId);
        printf("\t%s",user1.userName);
        printf("\t\t\t%d",user1.userAge);
    }
    fclose(filePtr);
}

void updateUser()
{
    int searchId,found=0;
    printf("Enter Id of User to Update: ");
    scanf("%d",&searchId);

    FILE *filePtr;
    filePtr = fopen("users.txt","r");
    if(filePtr == NULL)
    {
        printf("\nUnable to open File and update!");
        return;
    }

    FILE *filePtr2;
    filePtr2 = fopen("temp.txt","w");
    if(filePtr2 == NULL)
    {
        printf("\nUnable to open File and update!");
        fclose(filePtr);
        return;
    }

    struct users user1;
    
    while(fscanf(filePtr,"%d|%19[^|]|%d",&user1.userId,user1.userName,&user1.userAge) == 3)
    {
        if(searchId == user1.userId)
        {
            found = 1;
            printf("Enter new name: ");
            scanf(" %19[^\n]",user1.userName);
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF); 
            printf("Enter new Age: ");
            scanf("%d",&user1.userAge);
        }
        fprintf(filePtr2,"%d|%s|%d\n",user1.userId,user1.userName,user1.userAge);
    }
    fclose(filePtr);
    fclose(filePtr2);

    remove("users.txt");
    rename("temp.txt","users.txt");
    if(found == 1)
        printf("\nUser updated successfully");
    else
        printf("\nUser not found");
}

void deleteUser()
{
    int searchId, found=0;
    printf("Enter Id of User to Delete: ");
    scanf("%d",&searchId);

    FILE *filePtr;
    filePtr = fopen("users.txt","r");
    if(filePtr == NULL)
    {
        printf("\nUnable to open File and update!");
        return;
    }

    FILE *filePtr2;
    filePtr2 = fopen("temp.txt","w");
    if(filePtr2 == NULL)
    {
        printf("\nUnable to open File and update!");
        fclose(filePtr);
        return;
    }

    struct users user1;
    while(fscanf(filePtr,"%d|%19[^|]|%d",&user1.userId,user1.userName,&user1.userAge)==3)
    {
        if(searchId == user1.userId)
        {
            found = 1;
            continue;
        }
        fprintf(filePtr2,"%d|%s|%d\n",user1.userId,user1.userName,user1.userAge);
    }
    fclose(filePtr);
    fclose(filePtr2);
    remove("users.txt");
    rename("temp.txt","users.txt");
   
    if(found == 1)
        printf("\nUser deleted successfully");
    else
        printf("\nUser not found");
}

int lastId()
{
    FILE *filePtr;
    filePtr = fopen("users.txt","r");
    if(filePtr == NULL)
    {
        return -1;
    }
    struct users user1;
    int maxId=0;
    while(fscanf(filePtr,"%d|%19[^|]|%d",&user1.userId,user1.userName,&user1.userAge) == 3)
    {
        if(user1.userId > maxId)maxId=user1.userId;
    }
    fclose(filePtr);
    return maxId;
}
int main()
{
    createFile();

    int choice, userCount = lastId();
    if(userCount == -1)
    {
        printf("\nunable to open file and get last id!");return 0;
    }

    do
    {
        printf("\n---------File Menu-----------");
        printf("\n1. Create User\n2. Display File\n3. Update Record\n4. Delete Record\n5. Exit");

        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Enter a number.\n");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }
        switch (choice)
        {
            case 1: 
                    userCount++;    
                    createUser(userCount);
                break;
            case 2:displayUsers();
                break;
            case 3:updateUser();
                break;
            case 4:deleteUser();
                break;
            case 5:
                return 0;
            default:
                printf("Invalid Choice!");
        }
    } while (choice != 5);

    return 0;
}