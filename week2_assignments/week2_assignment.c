#include <stdio.h>

int main()
{
    char customerName[20];
    int ticketCount, categoryChoice;
    const float standard = 150, premium = 250, recliner = 400;
    float subtotal, discount = 0, ticketPrice;

    scanf(" %19s",customerName);
    while(1)
    {
        scanf("%d",&ticketCount);
        if(ticketCount >= 1 && ticketCount <= 20)
            break;

        printf("Invalid choice. Please enter ticket count in range 1 to 20: ");
    }
    while(1)
    {
        scanf("%d",&categoryChoice);
        if(categoryChoice >= 1 && categoryChoice <= 3)
            break;
        
        printf("Invalid choice. Please enter 1 for Standard, 2 for Premium, or 3 for Recliner: ");
    }

    printf("\nCustomer: %s", customerName);
    switch(categoryChoice)
    {
        case 1: ticketPrice = standard;
                printf("\nTicket Category: Standard");
                break;
        case 2: ticketPrice = premium;
                printf("\nTicket Category: Premium");
                break;
        case 3: ticketPrice = recliner;
                printf("\nTicket Category: Recliner");
                break;
    }
    printf("\nTickets: %d",ticketCount);
    subtotal = ticketCount * ticketPrice;
    printf("\nSubtotal: %.2f",subtotal);

    if(ticketCount >= 5)
        discount = (subtotal * 10)/100;
    printf("\nDiscount: %.2f",discount);
    printf("\nFinal Bill: %.2f", subtotal - discount);

    return 0;
}