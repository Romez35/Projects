//Pre-processing statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//Function Declaration
void OutputInit (void);

int main(void)

{
    //Declare variables and data types
    int PIN;
    int choice;
    float money;
    float balance = 1000.00;
    int i = 1;
    float deposit = 0.00;
    

    
    //Check the PIN entered
    while(1)
    {
        printf("Enter PIN #: ");
        scanf("%d", &PIN);
        
        if (PIN != 3344)
        {
            printf("Entered Wrong PIN\n");
        }
        else if (PIN == 3344)
        {
            break;
        }
    }
    
    while(1)
    {
        //Display options
        printf("\n\nSelect option\n 1. Withdraw\n 2. Deposit\n 3. Quit\n\n");
        scanf("%d", &choice);
        
        //Switch statement for choices
        switch(choice)
        {
            case(1):
                
                //Limit withdraws to 3 times
                if (i<4)
                {
                    printf("\nHow much to withdraw: ");
                    scanf("%f", &money);
                    
                    //Check if balance is greater than the amount entered
                    if (balance > money)
                    {
                        //Subtract money from balance
                        balance -= money;
                        printf("Balance remaining: $%.2f\n", balance);
                        
                        //Add 1 to counter
                        i++;
                    }
                    
                    else if (balance < money)
                    {
                        printf("Not enough funds!\n");
                    }
                    
                }
                
                //If withdrawal limit is reached
                else if (i == 4)
                {
                    printf("Too many withdraws made.\n");
                }
                
                break;
                
            case(2):
                
                printf("\nDeposit how much: ");
                scanf("%f", &money);
                
                //Add money to deposit amount to keep track
                deposit += money;

                //Check if deposit amount is within limit
                if (deposit >= 10000.00)
                {
                    printf("Reached max deposit limit.\n");
                    
                    //Remove money from deposit
                    deposit -= money;
                }
                
                else if (deposit < 10000.00)
                {

                    //Add money to balance
                    balance += money;
                    
                    printf("New Balance: %.2f", balance);
                }
                
                break;
                
            case(3):
                
                printf("Transactions Done.\n");
                exit(0);
                
            default:
                
                printf("Not valid option.\n");
                break;
        }
        
    }
    
}

