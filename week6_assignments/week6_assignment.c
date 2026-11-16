#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 10

struct FreeBlock
{
    int index;
    struct FreeBlock *prevFree;
    struct FreeBlock *nextFree;
};

struct FileNode
{
    char name[51];
    int isDirectory;

    struct FileNode *parent;
    struct FileNode *firstChild;

    struct FileNode *prevNode;
    struct FileNode *nextNode;

    int blockCount;
    int *blockPointers;
    size_t contentSize;
};

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

struct FreeBlock *freeHead = NULL;
struct FreeBlock *freeTail = NULL;

struct FileNode *root = NULL;
struct FileNode *cwd = NULL;

void freeFree()
{
    struct FreeBlock *temp = freeHead;
    while (temp != NULL)
    {
        freeHead = freeHead->nextFree;
        free(temp);
        temp = freeHead;
    }
    freeTail = NULL;
}

int initializeFreeList()
{
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        struct FreeBlock *newFree = malloc(sizeof(struct FreeBlock));
        if (newFree == NULL)
        {
            printf("Failed to create free list node!\n");
            freeFree();
            return 0;
        }
        if (freeHead == NULL)
        {
            freeHead = newFree;
        }
        else
        {
            freeTail->nextFree = newFree;
        }
        newFree->index = i;

        newFree->nextFree = NULL;
        newFree->prevFree = freeTail;

        freeTail = newFree;
    }

    root = calloc(1, sizeof(struct FileNode));
    if (root == NULL)
    {
        printf("Root directory initialization failed!\n");
        freeFree();
        return 0;
    }
    strcpy(root->name, "/");
    root->isDirectory = 1;
    root->parent = NULL;
    root->blockCount = 0;
    root->blockPointers = NULL;
    root->prevNode = NULL;
    root->contentSize = 0;
    root->firstChild = NULL;
    root->nextNode = NULL;

    cwd = root;
    return 1;
}

void displayFreeList()
{
    struct FreeBlock *temp = freeHead;
    int count = 0;
    while (temp != NULL)
    {
        printf("%d", temp->index);
        if (temp->nextFree != NULL)
        {
            printf("<->");
        }
        count++;
        temp = temp->nextFree;
    }
    printf("total free blocks %d\n", count);
}

int allocateFreeBlock()
{
    if (freeHead == NULL)
    {
        printf("Free list is empty!\n");
        return -1;
    }
    struct FreeBlock *temp = freeHead;
    int returnIndex = temp->index;
    freeHead = freeHead->nextFree;
    if (freeHead == NULL)
    {
        freeTail = NULL;
    }
    else
    {
        freeHead->prevFree = NULL;
    }
    free(temp);
    temp = NULL;
    return returnIndex;
}

int addToFreeList(int index)
{
    if (index < 0 || index >= NUM_BLOCKS)
    {
        printf("Invalid block index!\n");
        return 0;
    }

    struct FreeBlock *temp = freeHead;

    while (temp != NULL)
    {
        if (temp->index == index)
        {
            printf("Block is already free!\n");
            return 0;
        }

        temp = temp->nextFree;
    }
    struct FreeBlock *newNode = malloc(sizeof(struct FreeBlock));
    if (newNode == NULL)
    {
        printf("Node creation failed!\n");
        return 0;
    }
    newNode->nextFree = NULL;
    newNode->index = index;
    if (freeHead == NULL)
    {
        freeHead = newNode;
    }
    else
    {
        freeTail->nextFree = newNode;
    }
    newNode->prevFree = freeTail;

    freeTail = newNode;
    return 1;
}

int insertChild(struct FileNode *parent, struct FileNode *newNode)
{
    if (parent == NULL || newNode == NULL || !parent->isDirectory)
        return 0;

    if (parent->firstChild == NULL)
    {
        parent->firstChild = newNode;
        newNode->parent = parent;
        newNode->nextNode = newNode;
        newNode->prevNode = newNode;
    }
    else
    {
        newNode->parent = parent;
        struct FileNode *lastNode = parent->firstChild;
        while (lastNode->nextNode != parent->firstChild)
        {
            lastNode = lastNode->nextNode;
        }
        lastNode->nextNode = newNode;
        newNode->prevNode = lastNode;
        newNode->nextNode = parent->firstChild;
        parent->firstChild->prevNode = newNode;
    }
    return 1;
}

int removeChild(struct FileNode *parent, struct FileNode *target)
{
    if (parent == NULL || !parent->isDirectory || target == NULL || target->parent != parent)
    {
        return 0;
    }

    if (target->nextNode == target)
    {
        parent->firstChild = NULL;
    }
    else
    {
        target->prevNode->nextNode = target->nextNode;
        target->nextNode->prevNode = target->prevNode;
        if (target == parent->firstChild)
        {
            parent->firstChild = target->nextNode;
        }
    }
    target->prevNode = NULL;
    target->nextNode = NULL;
    target->parent = NULL;
    return 1;
}

int createDirectory(const char *name)
{
    if (cwd == NULL || !cwd->isDirectory || name == NULL || name[0] == '\0')
    {
        return 0;
    }

    if (strlen(name) > 50)
    {
        printf("Directory name is too long!\n");
        return 0;
    }

    if (cwd->firstChild != NULL)
    {
        struct FileNode *temp = cwd->firstChild;

        do
        {
            if (strcmp(temp->name, name) == 0)
            {
                printf("Name already exists!\n");
                return 0;
            }

            temp = temp->nextNode;

        } while (temp != cwd->firstChild);
    }

    struct FileNode *newDir = calloc(1, sizeof(struct FileNode));

    if (newDir == NULL)
    {
        printf("Failed to allocate memory!\n");
        return 0;
    }

    strcpy(newDir->name, name);
    newDir->isDirectory = 1;
    newDir->blockCount = 0;
    newDir->blockPointers = NULL;
    newDir->contentSize = 0;

    if (!insertChild(cwd, newDir))
    {
        printf("Failed to create directory!\n");
        free(newDir);
        return 0;
    }
    printf("Directory %s created successfully!\n", name);
    return 1;
}

int createFile(const char *name)
{
    if (cwd == NULL || !cwd->isDirectory || name == NULL || name[0] == '\0')
    {
        return 0;
    }
    if (strlen(name) > 50)
    {
        printf("File name is too long!\n");
        return 0;
    }
    if (cwd->firstChild != NULL)
    {
        struct FileNode *temp = cwd->firstChild;

        do
        {
            if (strcmp(temp->name, name) == 0)
            {
                printf("Name already exists!\n");
                return 0;
            }

            temp = temp->nextNode;

        } while (temp != cwd->firstChild);
    }
    struct FileNode *newFile = calloc(1, sizeof(struct FileNode));

    if (newFile == NULL)
    {
        printf("Failed to allocate memory!\n");
        return 0;
    }
    strcpy(newFile->name, name);
    newFile->isDirectory = 0;
    newFile->blockCount = 0;
    newFile->blockPointers = NULL;
    newFile->contentSize = 0;

    if (!insertChild(cwd, newFile))
    {
        printf("Failed to create file!\n");
        free(newFile);
        return 0;
    }
    printf("file %s created successfully!\n", name);
    return 1;
}

void listDirectory()
{
    if (cwd == NULL || !cwd->isDirectory)
    {
        printf("Invalid current directory!\n");
        return;
    }
    if (cwd->firstChild == NULL)
    {
        printf("[empty]\n");
        return;
    }

    struct FileNode *temp = cwd->firstChild;

    do
    {
        printf("%s ", temp->name);
        temp->isDirectory ? printf("[DIR]\n") : printf("[FILE]\n");
        temp = temp->nextNode;
    } while (temp != cwd->firstChild);
}

void changeDirectory(const char *name)
{
    if (cwd == NULL || !cwd->isDirectory || name == NULL)
    {
        printf("Invalid directory!\n");
        return;
    }
    if (!strcmp(name, ".."))
    {
        if (cwd->parent == NULL)
        {
            printf("Already at root directory!\n");
            return;
        }
        cwd = cwd->parent;
        printf("moved to parent directory %s\n", cwd->name);
        return;
    }
    else if (!strcmp(name, "/"))
    {
        if (cwd == root)
        {
            printf("Already in root directory!\n");
            return;
        }
        cwd = root;
        printf("moved to root directory %s\n", root->name);
        return;
    }
    if (cwd->firstChild == NULL)
    {
        printf("No any directory exist!\n");
        return;
    }

    struct FileNode *temp = cwd->firstChild;

    do
    {
        if (temp->isDirectory && (!strcmp(name, temp->name)))
        {
            cwd = temp;
            printf("moved to directory %s\n", cwd->name);
            return;
        }
        temp = temp->nextNode;
    } while (temp != cwd->firstChild);

    printf("Directory with name %s doesn't exist!\n", name);
    return;
}

void printWorkingDirectory(struct FileNode *cwd)
{
    if (cwd == NULL || cwd == root)
    {
        return;
    }
    printWorkingDirectory(cwd->parent);
    printf("/%s", cwd->name);
}
void pwd()
{
    if (cwd == root)
        printf("/");
    else
        printWorkingDirectory(cwd);
}

void deleteFile(const char *name)
{
    if (cwd == NULL || !cwd->isDirectory || name == NULL)
    {
        printf("Invalid operation!\n");
        return;
    }
    if (cwd->firstChild == NULL)
    {
        printf("No any file exist!\n");
        return;
    }
    struct FileNode *temp = cwd->firstChild;

    do
    {
        if (temp->isDirectory == 0 && !strcmp(temp->name, name))
        {
            break;
        }
        temp = temp->nextNode;
    } while (temp != cwd->firstChild);

    if (temp->isDirectory || strcmp(temp->name, name) != 0)
    {
        printf("File '%s' doesn't exist!\n", name);
        return;
    }

    for (int i = 0; i < temp->blockCount; i++)
    {
        if (!addToFreeList(temp->blockPointers[i]))
        {
            printf("Failed to return disk block!\n");
            return;
        }
    }
    removeChild(temp->parent, temp);

    free(temp->blockPointers);
    free(temp);

    printf("File '%s' deleted successfully!\n", name);
}

void removeDirectory(const char *name)
{
    if (cwd == NULL || !cwd->isDirectory || name == NULL)
    {
        printf("Invalid current working directory!\n");
        return;
    }
    if(!strcmp(name,"/"))
    {
        printf("root directory can't be removed!\n");
        return;
    }
    if (cwd->firstChild == NULL)
    {
        printf("Empty current working directory!\n");
        return;
    }
    struct FileNode *temp = cwd->firstChild;

    do
    {
        if (temp->isDirectory && !strcmp(temp->name, name))
        {
            if (temp->firstChild != NULL)
            {
                printf("Directory is not empty!\n");
                return;
            }
            removeChild(temp->parent, temp);
            free(temp);

            printf("Directory '%s' removed successfully!\n", name);
            return;
        }
        temp = temp->nextNode;
    } while (temp != cwd->firstChild);
    printf("Directory '%s' doesn't exist!\n", name);
}

void readFile(const char *name)
{
    if (cwd == NULL || !cwd->isDirectory || name == NULL)
    {
        printf("Invalid current working directory!\n");
        return;
    }
    if (cwd->firstChild == NULL)
    {
        printf("Empty current working Directory!\n");
        return;
    }
    struct FileNode *temp = cwd->firstChild;

    do
    {
        if (temp->isDirectory == 0 && !strcmp(temp->name, name))
        {
            if (temp->blockCount == 0)
            {
                printf("File is Empty!\n");
                return;
            }
            size_t remaining = temp->contentSize;
            for (int i = 0; i < temp->blockCount; i++)
            {
                size_t bytesToRead = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
                if (fwrite(virtualDisk[temp->blockPointers[i]], 1, bytesToRead, stdout) != bytesToRead)
                {
                    printf("failed to read!\n");
                    return;
                }
                remaining -= bytesToRead;
            }
            printf("\n");
            return;
        }
        temp = temp->nextNode;

    } while (temp != cwd->firstChild);

    printf("File %s not found!\n", name);
}

void writeFile(const char *name, const char *content)
{
    if (cwd == NULL || !cwd->isDirectory || name == NULL || name[0] == '\0')
    {
        return;
    }
    if (cwd->firstChild == NULL)
    {
        printf("Empty current working Directory!\n");
        return;
    }
    struct FileNode *temp = cwd->firstChild;
    if (content == NULL)
    {
        printf("No content found to write in File!\n");
        return;
    }
    size_t contentLength = strlen(content);
    do
    {
        if (!strcmp(temp->name, name))
        {
            if (temp->isDirectory == 1)
            {
                printf("Cannot write to a Directory!\n");
                return;
            }
            if (temp->contentSize > 0)
            {
                printf("file already written!\n");
                return;
            }
            size_t requiredBlocks = (contentLength + BLOCK_SIZE - 1) / BLOCK_SIZE;
            if (contentLength == 0)
            {
                temp->blockPointers = NULL;
                temp->blockCount = 0;
                temp->contentSize = 0;

                printf("Empty file created successfully!\n");
                return;
            }
            if (freeHead == NULL)
            {
                printf("No free block available!\n");
                return;
            }
            struct FreeBlock *tempFree = freeHead;
            size_t count = 0;
            do
            {
                count++;
                tempFree = tempFree->nextFree;
            } while (tempFree != NULL);

            if (count < requiredBlocks)
            {
                printf("No enough free Blocks are available!\n");
                return;
            }

            temp->blockPointers = calloc(requiredBlocks, sizeof(*temp->blockPointers));
            if (temp->blockPointers == NULL)
            {
                printf("failed to assign free blocks!\n");
                return;
            }
            size_t i;
            for (i = 0; i < requiredBlocks; i++)
            {
                int index = allocateFreeBlock();
                if (index == -1)
                {
                    for (size_t j = 0; j < i; j++)
                    {
                        if (!addToFreeList(temp->blockPointers[j]))
                        {
                            printf("failed to rollback!\n");
                            free(temp->blockPointers);
                            temp->blockPointers = NULL;
                            return;
                        }
                    }
                    printf("failed to allocate free block!\n");
                    free(temp->blockPointers);
                    temp->blockPointers = NULL;
                    return;
                }
                temp->blockPointers[i] = index;
            }

            size_t remaining = contentLength;
            size_t offset = 0;
            for (size_t i = 0; i < requiredBlocks; i++)
            {
                size_t bytesToCopy = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

                int blockIndex = temp->blockPointers[i];

                memcpy(virtualDisk[blockIndex], content + offset, bytesToCopy);

                offset += bytesToCopy;
                remaining -= bytesToCopy;
            }

            temp->blockCount = requiredBlocks;
            temp->contentSize = contentLength;
            printf("Content written successfully!\n");
            return;
        }
        temp = temp->nextNode;
    } while (temp != cwd->firstChild);

    printf("%s file not found!\n", name);
}

void displayDiskUsage()
{
    int freeCount = 0;
    struct FreeBlock *temp = freeHead;

    while (temp != NULL)
    {
        freeCount++;
        temp = temp->nextFree;
    }

    int usedBlocks = NUM_BLOCKS - freeCount;

    float diskUsage = ((float)usedBlocks / NUM_BLOCKS) * 100;

    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Free Blocks: %d\n", freeCount);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Disk Usage: %.2f%%\n", diskUsage);
}

void cleanupDirectory(struct FileNode *directory)
{
    if (directory == NULL)
        return;

    while (directory->firstChild != NULL)
    {
        struct FileNode *child = directory->firstChild;
        if (!removeChild(directory, child))
        {
            printf("Failed to detach child during cleanup!\n");
            return;
        }

        if (child->isDirectory)
        {
            cleanupDirectory(child);
        }
        else
        {
            free(child->blockPointers);
        }

        free(child);
    }
}

void cleanupFileSystem()
{
    cleanupDirectory(root);

    free(root);
    root = NULL;
    cwd = NULL;

    freeFree();

    printf("Memory released. Exiting program....\n");
}

int main()
{
    if (!initializeFreeList())
    {
        printf("VFS initialization failed!\n");
        return 1;
    }
    printf("$./vfs\nCompact VFS ready. Type exit to quit.\n");
    while (1)
    {
        char command[7];
        char argument[51];
        char content[5121];
        pwd();
        printf("> ");
        if (scanf("%6s", command) != 1)
        {
            printf("Invalid command!\n");
            break;
        }
        if (!strcmp(command, "ls"))
        {
            listDirectory();
        }
        else if (!strcmp(command, "pwd"))
        {
            pwd();
            printf("\n");
        }
        else if (!strcmp(command, "df"))
        {
            displayDiskUsage();
        }
        else if (!strcmp(command, "exit"))
        {
            break;
        }
        else if (strcmp(command, "mkdir") == 0)
        {
            if (scanf(" %50s", argument) != 1)
            {
                printf("Directory name missing!\n");
                continue;
            }

            createDirectory(argument);
        }
        else if (strcmp(command, "create") == 0)
        {
            if (scanf(" %50s", argument) != 1)
            {
                printf("File name missing!\n");
                continue;
            }

            createFile(argument);
        }
        else if (strcmp(command, "cd") == 0)
        {
            if (scanf(" %50s", argument) != 1)
            {
                printf("Directory name missing!\n");
                continue;
            }

            changeDirectory(argument);
        }
        else if (strcmp(command, "write") == 0)
        {
            if (scanf(" %50s", argument) != 1)
            {
                printf("Invalid file name!\n");
                continue;
            }

            int ch;

            do
            {
                ch = getchar();
            } while (ch == ' ' || ch == '\t');

            if (ch == '"')
            {
                if (fgets(content, sizeof(content), stdin) == NULL)
                {
                    printf("Failed to read content!\n");
                    continue;
                }
                size_t length = strlen(content);

                if (length > 0 && content[length - 1] == '\n')
                {
                    content[length - 1] = '\0';
                }
                else
                {
                    int ch;

                    while ((ch = getchar()) != '\n' && ch != EOF)
                    {
                        
                    }
                }

                char *closingQuote = strchr(content, '"');

                if (closingQuote == NULL)
                {
                    printf("Missing closing double quote or longer content!\n");
                    continue;
                }

                *closingQuote = '\0';

                char *extra = closingQuote + 1;

                while (*extra == ' ' || *extra == '\t')
                {
                    extra++;
                }

                if (*extra != '\0' && *extra != '\n')
                {
                    printf("Unexpected characters after closing quote!\n");
                    continue;
                }
            }
            else
            {
                if (ch == '\n' || ch == EOF)
                {
                    content[0] = '\0';
                }
                else
                {
                    ungetc(ch, stdin);

                    if (fgets(content, sizeof(content), stdin) == NULL)
                    {
                        printf("Failed to read content!\n");
                        continue;
                    }
                    size_t length = strlen(content);

                if (length > 0 && content[length - 1] == '\n')
                {
                    content[length - 1] = '\0';
                }
                else
                {
                    int ch;

                    while ((ch = getchar()) != '\n' && ch != EOF)
                    {
                        
                    }
                }

                    content[strcspn(content, "\n")] = '\0';
                }
            }

            writeFile(argument, content);
        }
        else if (strcmp(command, "read") == 0)
        {
            if (scanf(" %50s", argument) != 1)
            {
                printf("File name missing!\n");
                continue;
            }

            readFile(argument);
        }
        else if (strcmp(command, "delete") == 0)
        {
            if (scanf(" %50s", argument) != 1)
            {
                printf("File name missing!\n");
                continue;
            }

            deleteFile(argument);
        }
        else if (strcmp(command, "rmdir") == 0)
        {
            if (scanf(" %50s", argument) != 1)
            {
                printf("Directory name missing!\n");
                continue;
            }

            removeDirectory(argument);
        }
        else
        {
            printf("Unknown command: %s\n", command);
        }
    }
    cleanupFileSystem();
    return 0;
}