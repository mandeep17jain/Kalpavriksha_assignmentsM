#include <stdio.h>
int getNumber(char str[], int *pos)
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
    char str[100],ch=0;
    str[0]='0';
    int i = 1;
    printf("Enter expression: ");
    while (i < 100 && ch != '\n') 
    {
        scanf("%c", &ch);
        if(ch == ' ' || ch == '\t')continue;
        if(ch != '/' && ch != '*' && ch != '+' && ch != '-' && ch !='\n' && (ch < 48 || ch > 57))
        {
            printf("Error: Invalid expression");return 0;
        }
        str[i++] = ch;
    }
    if(str[i-2] == '/' || str[i-2] == '*' || str[i-2] == '+' || str[i-2] == '-')
    {
        i--;
    }
    str[--i]='\0';

    int pos = 0;
    char operatorStack[100];
    int operandStack[100] ;
    int operatorTop = -1, operandTop = -1;

    operandStack[++operandTop]  = getNumber(str, &pos);
    while (str[pos] != '\0')
    {
        if (str[pos] == '/')
        {
            pos++;
            int num = getNumber(str,&pos);
            if (num)
                operandStack[operandTop] = operandStack[operandTop] / num; 
            else
            {
                printf("Error: Division by zero");
                return 0;
            }                
        }
        else if (str[pos] == '*')
        {
            pos++;
            operandStack[operandTop] = operandStack[operandTop] * getNumber(str, &pos); 
        }
        else if(str[pos] == '+')
         {
                operatorStack[++operatorTop]='+';pos++; 
         }   
        else if(str[pos] == '-')
            {operatorStack[++operatorTop]='-'; pos++;} 
        else 
        {
            operandStack[++operandTop] = getNumber(str, &pos);
        }    
    }

    int result = operandStack[0],digit=1;
    for(int s=0;s<=operatorTop;s++)
    {
        if(operatorStack[s] == '+')
            result = result+operandStack[digit++];
        else if(operatorStack[s] == '-')
            result = result-operandStack[digit++];
    }
    printf("Result: %d",result);
    return 0;
}