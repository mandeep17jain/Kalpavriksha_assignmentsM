#include <stdio.h>
struct user
{
    int id;
    char name[20];
    int age;
};
void createFile()
{
    FILE *fptr;
    fptr = fopen("users.txt","a");
    if(fptr==NULL)
    {
        printf("Unable to create File!");
        return;
    }
    printf("File created successfully!");
    fclose(fptr);
}
void createUser(int countUser)
{
    struct user u;
    
    u.id=countUser;
    printf("Enter user name: ");
    scanf("%s",u.name);
    printf("Enter user Age: ");
    scanf("%d",&u.age);

    FILE *fptr;
    fptr = fopen("users.txt","a");
    if(fptr==NULL)
    {
        printf("\nUnable to open File!");
        return;
    }
    fprintf(fptr,"%d %s %d\n",u.id,u.name,u.age);
    printf("\nUser created successfully!");
    fclose(fptr);
}

void Display()
{
    FILE *fptr;
    fptr = fopen("users.txt","r");
    if(fptr==NULL)
    {
        printf("\nUnable to open File!");
        return;
    }
    struct user u;
    printf("\n----------Users Lists----------");
    printf("\nID\tName\t\tAge");
    printf("\n--------------------------------");
    while(fscanf(fptr,"%d %s %d",&u.id,u.name,&u.age)==3)
    {
        printf("\n%d",u.id);
        printf("\t%s",u.name);
        printf("\t\t%d",u.age);
    }
    fclose(fptr);
}

void Update()
{
    int searchId,found=0;
    printf("Enter Id of User to Update: ");
    scanf("%d",&searchId);

    FILE *fptr;
    fptr = fopen("users.txt","r");
    if(fptr==NULL)
    {
        printf("\nUnable to open File and update!");
        return;
    }

    FILE *fptr2;
    fptr2 = fopen("temp.txt","w");
    if(fptr2==NULL)
    {
        printf("\nUnable to open File and update!");
        return;
    }

    struct user u;
    
    while(fscanf(fptr,"%d %s %d",&u.id,u.name,&u.age)==3)
    {
        if(searchId == u.id)
        {
            found =1;
            printf("\nEnter new name: ");
            scanf("%s",&u.name);
            printf("\nEnter new age: ");
            scanf("%d",&u.age);
        }
        fprintf(fptr2,"%d %s %d\n",u.id,u.name,u.age);
    }
    fclose(fptr);
    fclose(fptr2);

    remove("users.txt");
    rename("temp.txt","users.txt");
    if(found == 1)
        printf("\nUser updated successfully");
    else
        printf("\nUser not found");
}

void DeleteU()
{
    int searchId,found=0;
    printf("Enter Id of User to Delete: ");
    scanf("%d",&searchId);

    FILE *fptr;
    fptr = fopen("users.txt","r");
    if(fptr==NULL)
    {
        printf("\nUnable to open File and update!");
        return;
    }

    FILE *fptr2;
    fptr2 = fopen("temp.txt","w");
    if(fptr2==NULL)
    {
        printf("\nUnable to open File and update!");
        return;
    }

    struct user u;
    while(fscanf(fptr,"%d %s %d",&u.id,u.name,&u.age)==3)
    {
        if(searchId == u.id)
        {
            found =1;
            continue;
        }
        fprintf(fptr2,"%d %s %d\n",u.id,u.name,u.age);
    }
    fclose(fptr);
     fclose(fptr2);
    remove("users.txt");
    rename("temp.txt","users.txt");
   
    if(found == 1)
        printf("\nUser deleted successfully");
    else
        printf("\nUser not found");
}

int lastId()//to get last id so each user has unique id
{
    FILE *fptr;
    fptr = fopen("users.txt","r");
    if(fptr==NULL)
    {
        return -1;
    }
    struct user u;
    int c=0;
    while(fscanf(fptr,"%d %s %d",&u.id,u.name,&u.age)==3)
    {
        c++;
    }
    fclose(fptr);
    if(c>=1)
        return u.id;
    return 0;
    
}
int main()
{
    createFile();

    int choice,countUser=lastId();
    if(countUser==-1)
    {
        printf("\nunable to open file and get last id!");return 0;
    }

    do
    {
        printf("\n---------File Menu-----------");
        printf("\n1. Create User\n2. Display File\n3. Update Record\n4. Delete Record\n5. Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1: countUser++; createUser(countUser);
                break;
            case 2:Display();
                break;
            case 3:Update();
                break;
            case 4:DeleteU();
                break;
            case 5:
                return 0;
                break;
            default:
                printf("Invalid Choice!");
        }
    } while (choice != 5);

    return 0;
}