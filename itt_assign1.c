#include <stdio.h>
int getnumber(char str[], int *pos)
{
    int num = str[*pos] - '0';
    (*pos)++;
    while (str[*pos] - '0' >= 0 && str[*pos] - '0' <= 9)
    {
        num = num * 10 + (str[*pos] - '0');
        (*pos)++;
    }
    return num;
}

int main()
{
    char str[100],ch;
    str[0]='0';
    int i = 1;
    printf("0");
    
    while (i < 100 && ch != '\n') //take elements until enter hits, and ignore whitespaces
    {
        scanf("%c", &ch);
        if(ch == ' ' || ch == '\t')continue;
        if(ch != '/' && ch != '*' && ch != '+' && ch != '-' && ch !='\n' && (ch < 48 || ch > 57))
        {
            printf("Error: Invalid expression");return 0;
        }
        str[i++] = ch;
    }
    str[--i]='\0'; //replace '\n' with '\0'

    int pos = 0;
    //two stacks, for operators (+ and -) and for numbers to be operated with + and -
    char stChar[100], topChar = -1;
    int stInt[100], topInt = -1;
    stInt[++topInt]  = getnumber(str, &pos);
    
    while (str[pos] != '\0')
    {
        if (str[pos] == '/')
        {
            pos++;
            int num = getnumber(str,&pos);
            if (num)
                stInt[topInt] = stInt[topInt] / num; //solve immediately and store new result at top
            else
            {
                printf("Error: Division by zero");
                return 0;
            }                
        }
        else if (str[pos] == '*')
        {
            pos++;
            stInt[topInt] = stInt[topInt] * getnumber(str, &pos); //solve immediately and store new result at top
        }
        else if(str[pos] == '+')
         {
                stChar[++topChar]='+';pos++; //store operator in stChar 
         }   
        else if(str[pos] == '-')
            {stChar[++topChar]='-'; pos++;} //store operator in stChar 
        else 
        {
            stInt[++topInt] = getnumber(str, &pos);
        }    
    }

    int result = stInt[0],digit=1;
    for(int s=0;s<=topChar;s++) //lastly, solve remaining numbers to be operated with + and - 
    {
        if(stChar[s] == '+')
            result = result+stInt[digit++];
        else if(stChar[s] == '-')
            result = result-stInt[digit++];
    }
    printf("Result: %d",result);
    return 0;
}