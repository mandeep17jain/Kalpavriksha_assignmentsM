#include <stdio.h>
const float standard = 150, premium = 250, recliner = 400;
void clearInputBuffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}
float getTicketPrice(int categoryChoice)
{
    switch(categoryChoice)
    {
        case 1: 
                printf("\nTicket Category: Standard");
                return standard;
                
        case 2: 
                printf("\nTicket Category: Premium");
                return premium;

        case 3: 
                printf("\nTicket Category: Recliner");
                return recliner;
    }
}
float calculateDiscount(float subtotal)
{    
    return (subtotal * 10)/100;
}
int main()
{
    char customerName[20];
    int ticketCount, categoryChoice;
    float subtotal, discount = 0, ticketPrice;

    scanf(" %19s",customerName);
    clearInputBuffer();
    while(1)
    {
        if(scanf("%d",&ticketCount) != 1 ||  (ticketCount < 1 || ticketCount > 20))
        {
            printf("Invalid choice. Please enter ticket count in range 1 to 20: ");
            clearInputBuffer();
        }
        else 
            break;
    }
    while(1)
    {
        if(scanf("%d",&categoryChoice) != 1 || (categoryChoice < 1 || categoryChoice > 3))
        {
            printf("Invalid choice. Please enter category 1 for Standard, 2 for Premium, or 3 for Recliner: ");
            clearInputBuffer();
        }
        else
            break;
    }

    printf("\nCustomer: %s", customerName);

    ticketPrice = getTicketPrice(categoryChoice);

    printf("\nTickets: %d",ticketCount);

    subtotal = ticketCount * ticketPrice;
    printf("\nSubtotal: %.2f",subtotal);

    if(ticketCount >= 5)
        discount = calculateDiscount(subtotal);
        
    printf("\nDiscount: %.2f",discount);

    printf("\nFinal Bill: %.2f", subtotal - discount);

    return 0;
}