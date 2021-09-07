//Place header files as needed
#include <msp432p401r.h>
#include <stdlib.h>
#include <stdio.h>

//Function for transmitting
void Tx(char word[30])
{
	int i = 0;
	while (word[i] != 0)
	{
		EUSCI_A0 -> TXBUF = word[i];
		while((EUSCI_A0 -> IFG & 2)==0)
		{
			//wait
		}
		i++;
	}
}

//Function for recieving number inputs
int Rx(void)
{
	int i = 0;
	int x;
	char word[30];
	
	while(1)
	{
		if((EUSCI_A0 ->IFG & 1) != 0)
		{
			word[i] = EUSCI_A0 ->RXBUF; //Place current character in array
			EUSCI_A0 -> TXBUF = word[i];
			while((EUSCI_A0 -> IFG & 2) ==0)
			{}
			if(word[i] == '\r') //Enter key pressed
			{
				word[i] = '\0'; 
				break; //break out of while loop
			}
			else
			{
				i++; //increment index
			}
		}
	}
	
	x = atoi(word); //Convert string to integer
	return x;
}
	
void ADCInit(void);//initialize the ADC

float tempRead(void); //read temperature sensor

//function definitions
void ADCInit(void)
{
	//Ref_A settings
	REF_A ->CTL0 &= ~0x8; //enable temp sensor
	REF_A ->CTL0 |= 0x30; //set ref voltage
	REF_A ->CTL0 &= ~0x01; //enable ref voltage
	
	//do ADC stuff
	ADC14 ->CTL0 |= 0x10; //turn on the ADC
	ADC14 ->CTL0 &= ~0x02; //disable ADC
	ADC14 ->CTL0 |=0x4180700; //no prescale, mclk, 192 SHT
	ADC14 ->CTL1 &= ~0x1F0000; //configure memory register 0
	ADC14 ->CTL1 |= 0x800000; //route temp sense
	ADC14 ->MCTL[0] |= 0x100; //vref pos int buffer
	ADC14 ->MCTL[0] |= 22; //channel 22
	ADC14 ->CTL0 |=0x02; //enable adc
		
	return;
} 

float tempRead(void)
{
	float temp; //temperature variable
	uint32_t cal30 = TLV ->ADC14_REF2P5V_TS30C; //calibration constant
	uint32_t cal85 = TLV ->ADC14_REF2P5V_TS85C; //calibration constant
	float calDiff = cal85 - cal30; //calibration difference
	ADC14 ->CTL0 |= 0x01; //start conversion
	while((ADC14 ->IFGR0) ==0) 
	{
		//wait for conversion
	}
	temp = ADC14 ->MEM[0]; //assign ADC value
	temp = (temp - cal30) * 55; //math for temperature per manual
	temp = (temp/calDiff) + 30; //math for temperature per manual
	
	return temp; //return temperature in degrees C
}

void Tick(float t) //Function for SysTick Timer
	
{
	
	SysTick -> LOAD = ((t * 3000000) - 1); //Delay by t seconds
	SysTick -> CTRL |= 0x04; //Enable SysTick
	SysTick -> CTRL |= 0x01; //Enable SysTick
				
	while((SysTick -> CTRL & 0x10000) == 0) //while COUNTFLAG not set
	{
		//wait
	}
	
	SysTick -> CTRL &= ~0x01; //Disable SysTick
	return;
}

void Timer32(float t) //Function for Timer32
	
{
	
	TIMER32_1 -> LOAD = ((t * 3000000) - 1); //Delay by t seconds
	TIMER32_1 -> CONTROL |= 0x43; //No interrupt or prescale, 32-bit size, wrapping mode
	TIMER32_1 -> CONTROL |= 0x80; //Enable Timer
	
	while((TIMER32_1 -> RIS & 1) ==0) //While count not done
	{
		//wait
	}
	
	TIMER32_1 -> INTCLR &= ~0x01; //Set INTCLR to 0 after count is reached
	return;
}

void RGB(int comb, int toggle, int blink)
{
	int i;
	
	P2 -> SEL0 &= ~0x07; //Specify port 0 as Digital I/O
	P2 -> SEL1 &= ~0x07; //Specify port 1 as Digital I/O
	P2 -> DIR |= 0x07; //Make pins 1,2,3 OUTPUT
	
	for(i = 0; i < blink; i++)
	{
		P2 -> OUT &= ~0x07;
		Tick(toggle);
		P2 -> OUT |= comb;
		Tick(toggle);
	}
	P2 -> OUT &= ~0x07; //Clear buffer
}

void button(void)
{
	P1 -> SEL0 &= ~0x12; // pin 1 and pin 4 on select 0
	P1 -> SEL1 &= ~0x12; // pin 1 and pin 4 on select 1
	P1 -> DIR &= ~0x12; 
	P1 -> REN |= 0x12;
	
	P1 -> OUT |= 0x12; //Pins 2 and 4 generate a HIGH
	P1 -> DIR |= 0x01; //Make pin 0 OUPUT
	
	if (((P1 -> IN & 0x02) == 2) && ((P1 -> IN & 0x10) == 16)) //If nothing is pressed
	{
		Tx("No Button pressed.\n");
	}
	
	else if ((P1 -> IN & 0x02) == 0 && ((P1 -> IN & 0x10) == 16)) //If S1 is pressed and S2 is not pressed
	{
		Tx("Button 1 is pressed\n");
	}
		
	else if (((P1 -> IN & 0x10) == 0) && (P1 -> IN & 0x02) == 2) //If S2 is pressed and S1 is not pressed
	{
		Tx("Button 2 is pressed\n");
	}
	
	else if (((P1 -> IN & 0x02) == 0) && ((P1 -> IN & 0x10) == 0)) //If both buttons are pressed
	{
		Tx("Both buttons are pressed\n");
	}
	Tx("\n\r");
}


int main(void)
{
	//Declare vairables and data types
	int choice;
	int x;
	int input;
	int comb;
	int toggle;
	int blink;
	float c; //Celcius
	float f; //Fahrenheit
	
	//Initialize UART
	EUSCI_A0 -> CTLW0 |= 1; //put in reset state
	EUSCI_A0 -> MCTLW = 0;
	EUSCI_A0 -> CTLW0 |= 0x80; //1 stop bit, no parity, SMCLK, 8-bit data
	EUSCI_A0 -> BRW = 26; //baud rate
	P1 -> SEL0 |= 0x0C;
	P1 -> SEL1 &= ~0x0C;
	EUSCI_A0 -> CTLW0 &= ~0x01; //Take out of reset state
	
	while(1)
	{	
	//Display Menu
	Tx("MSP432 Menu\n\n\r");
	Tx("1. RGB Control\n\r");
	Tx("2. Digital Input\n\r");
	Tx("3. Temperature Reading\n\r");
	choice = Rx();
	
	
		switch(choice)
		{
			case(1): //Option 1
			{
				Tx("\nEnter combination of RGB (1-7): ");
				comb = Rx();
				Tx("\nEnter Toggle Time: ");
				toggle = Rx();
				Tx("\nEnter Number of Blinks: ");
				blink = Rx();
				Tx("\nBlinking LED...\n\r");
				RGB(comb, toggle, blink);
				Tx("Done\n\n\r");
			break;
			}
			
			case(2): //Option 2
			{
				button();
			break;
			}
			
			case(3): //Option 3
			{
			Tx("\nEnter Number of Temperature reading (1-5): ");
			input = Rx();
			ADCInit();
			for (x = 0; x < input; x++)
				{
					char text[30];
					c = tempRead(); //Read celcius
					f = (float)(tempRead() * 9/5 + 32); //Convert temp to fahrenheit
					sprintf(text, "\n\rReading %d: %.2f C & %.2f F", x+1, c, f);
					Tx(text);
					Timer32(1);
				}
				Tx("\n\n\r");
			break;
			}
			
			default:
			{
			Tx("\nInvalid choice\n\n\r");
			
			}
		}
	}
	
}


