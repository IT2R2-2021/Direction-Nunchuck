/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stdio.h"
#include "Driver_SPI.h"                 // ARM::CMSIS Driver:SPI:Custom
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "LPC17xx.h"                    // Device header
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "GPIO.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Driver_SPI.h"                 // ARM::CMSIS Driver:SPI:Custom


#define CYCLE_TIME 255


extern ARM_DRIVER_USART Driver_USART1;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;


uint8_t dataLue[10];
int mr,etat,mrtest;
double alpha;

void TIMER0_IRQHandler(void)
{
	LPC_TIM0->IR=LPC_TIM0->IR | (1<<0);
	LPC_TIM0->TCR=0;
	if (etat==3)
	{
		if(mr>100000)
		{
			mr=  alpha*500000;
		}
		else if (mr<100000)
		{
			mr= 500000-(alpha*500000);
		}
	}
	else if (etat==2)
	{
		if(mr>100000)
		{
			mr=  alpha*500000;
		}
		else if (mr<100000)
		{
			mr= 500000-(alpha*500000);
		}
	}
	else if (etat==1)
	{
		if(mr>100000)
		{
			mr= alpha*500000;
		}
		else if (mr<100000)
		{
			mr= 500000-(alpha*500000);
		}
	}
	

	LPC_GPIO2->FIOPIN0=LPC_GPIO2->FIOPIN0^(1<<4);
	LPC_TIM0->MR0=mr;
	
	LPC_TIM0->TCR=1;
}

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}
void tacheDirection(void const *argument);

osThreadId ID_tacheDirection;

void tacheDirection(void const *argument)
{
	int i,ratio=19;
	char RxMsg[20];
	char valeurx[3];
	char valeury[3];
	unsigned char char_x=0,char_y=0;
	char affichagex[20];
	char affichagey[20];
	while(1)
	{
		

		alpha= ((((char_x-255.0)*(-1.0))*(0.028/255.0))+0.057);
		GLCD_DrawString(1,60,"grdgrd");
		Driver_USART1.Receive(RxMsg,18);
		while(Driver_USART1.GetRxCount()<18);
		
		GLCD_DrawString(1,60,"grdgrd");
		
		for (i=1;RxMsg[i-1]!='x';i++)
		{
			if(RxMsg[i]=='x')
			{
				valeurx[0]=RxMsg[i+1];
				valeurx[1]=RxMsg[i+2];
				valeurx[2]=RxMsg[i+3];
				valeury[0]=RxMsg[i+4];
				valeury[1]=RxMsg[i+5];
				valeury[2]=RxMsg[i+6];				
			}
		}
		char_x=(valeurx[0]&0x0F)*100+(valeurx[1]&0x0F)*10+(valeurx[2]&0x0F);
		//char_x=0;
		char_y=(valeury[0]&0x0F)*100+(valeury[1]&0x0F)*10+(valeury[2]&0x0F);
		sprintf(affichagex," %d       ",char_x);
		sprintf(affichagey," %d       ",char_y);
		
		
		if (char_x<=128)
		{
			etat=1;
			if (mr>100000)
			{				
				mr=500000-(alpha*500000);
			}
			else
			{
				mr=  alpha*500000;
			}
			
		}
		else if (char_x>=133)
		{
			
			etat=2;
			if (mr>100000)
			{
				mr= 500000-(alpha*500000);
			}
			else
			{
				mr=  alpha*500000;
			}
		}
		else if ((char_x<133) && (char_x>128))
		{
			etat=3;
			if (mr>100000)
			{
				mr= 500000-(alpha*500000);
			}
			else
			{
				mr=  alpha*500000;
			}
		} 
		
		
		if (char_y<=85)
		{
			GPIO_PinWrite(0,16,false);
			GPIO_PinWrite(0,17,true);
			LPC_PWM1->MR3=1250;
			LPC_PWM1->MR2=1250;
		}
		else if (char_y>=134)
		{
			GPIO_PinWrite(0,16,true);
			GPIO_PinWrite(0,17,false);
			LPC_PWM1->MR3=1250;
			LPC_PWM1->MR2=1250;
		}
		else if ((char_y<170) && (char_x>85))
		{
			GPIO_PinWrite(0,16,true);
			GPIO_PinWrite(0,17,false);
			LPC_PWM1->MR3=0;
			LPC_PWM1->MR2=0;
		}
		
		
		//(int) (((((char_x-255)*(-1))*(0.031/255))+0.057)*500000)-1
		//(int) (((((char_x-255)*(-1))*(0.031/255))+0.057)*500000)-1
		//mrtest = (int) (((((char_x-255)*(-1))*(0.031/255))+0.057)*500000);
		//GLCD_DrawString(1,1,valeurx);
		GLCD_DrawString(100,1,affichagex);
		//GLCD_DrawString(1,30,valeury);
		GLCD_DrawString(100,30,affichagey);
		

	}
	
}
	

osThreadDef (tacheDirection, osPriorityNormal, 1,0) ;


/*
 * main: initialize and start the system
 */
int main (void) {
	
  osKernelInitialize ();  
	SystemInit();
	Initialise_GPIO ();
	
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	
	Init_UART();
	LPC_GPIO2->FIOPIN0=LPC_GPIO2->FIOPIN0 & (0<<4);
	mr=456000;
	mrtest=250000;
	GPIO_SetDir(0,16,GPIO_DIR_OUTPUT);
	GPIO_SetDir(0,17,GPIO_DIR_OUTPUT);
	
	//PWM1
	LPC_SC->PCONP = LPC_SC->PCONP | (1<<6); //Activation du PWM1
	
	LPC_PWM1->PR=0;
	LPC_PWM1->MR0=2499;
	
	
	LPC_PWM1->MR3=1250;
	LPC_PWM1->MR2=1250;
	
	
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 | (3<<18);
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 | 0x00300000;
	
	LPC_PWM1->MCR = LPC_PWM1->MCR | 0x00000002;
	LPC_PWM1->LER = LPC_PWM1->LER | 0x00000005;

	LPC_PWM1->LER = LPC_PWM1->LER | 0x00000009;
	
	LPC_PWM1->PCR = LPC_PWM1->PCR  | 0x00000e00;
	LPC_PWM1->PCR = LPC_PWM1->PCR  | (1<<10);
	LPC_PWM1->TCR = 1;
	
	//PWM INT
	LPC_GPIO2->FIODIR0=LPC_GPIO2->FIODIR0 | (1<<4);
	LPC_TIM0->PR=0;
	LPC_TIM0->MR0=mr;
	LPC_TIM0->MCR=LPC_TIM0->MCR | (3<<0);
	LPC_TIM0->TCR=1;
	
	NVIC_SetPriority(TIMER0_IRQn,0);
	NVIC_EnableIRQ(TIMER0_IRQn);
	

  ID_tacheDirection = osThreadCreate (osThread (tacheDirection), NULL) ;
	
	
  osKernelStart ();                         // start thread execution 
	osDelay (osWaitForever) ;
	return 0;
}
