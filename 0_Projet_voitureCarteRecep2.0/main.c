/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stdio.h"
#include "Driver_SPI.h"                 // ARM::CMSIS Driver:SPI:Custom
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "LPC17xx.h"                    // Device header
#include "delay.h"
#include "pwm.h"
#include "gpio.h"



#define CYCLE_TIME 255


extern ARM_DRIVER_USART Driver_USART1;



uint8_t dataLue[10];

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
void tache1(void const *argument);

osThreadId ID_tache1;

void tache1(void const *argument)
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
		
		PWM_SetDutyCycle( PWM_5, ratio );
		
		
		Driver_USART1.Receive(RxMsg,18);
		while(Driver_USART1.GetRxCount()<18);
		
//		GLCD_DrawString(1,60,RxMsg);
		
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
		char_y=(valeury[0]&0x0F)*100+(valeury[1]&0x0F)*10+(valeury[2]&0x0F);
		/*sprintf(affichagex," %d       ",char_x);
		sprintf(affichagey," %d       ",char_y);
		
		
		GLCD_DrawString(1,1,valeurx);
		GLCD_DrawString(100,1,affichagex);
		GLCD_DrawString(1,30,valeury);
		GLCD_DrawString(100,30,affichagey);*/
		
	
		if ((ratio!=22)&&(char_x<=32))
		{
			ratio=22;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		else if ((ratio!=21)&&(char_x>32) && (char_x<=64))
		{
			ratio=21;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		else if ((ratio!=20)&&(char_x>64) && (char_x<=96))
		{
			ratio=20;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		else if ((ratio!=19)&&(char_x>96) && (char_x<=128))
		{
			ratio=19;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		else if ((ratio!=18)&&(char_x>128) && (char_x<=160))
		{
			ratio=18;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		else if ((ratio!=17)&&(char_x>160) && (char_x<=192))
		{
			ratio=17;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		else if ((ratio!=16)&&(char_x>192) && (char_x<=224))
		{
			ratio=16;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		else if ((ratio!=15)&&(char_x>224) && (char_x<=255))
		{
			ratio=15;
			PWM_SetDutyCycle( PWM_5, ratio );
		}
		
		//osDelay(500);

	}
	
}
	

osThreadDef (tache1, osPriorityNormal, 1,0) ;


/*
 * main: initialize and start the system
 */
int main (void) {
	
  osKernelInitialize ();  
	SystemInit();
  PWM_Init(255);          
	PWM_Start(PWM_5);
	
	GPIO_PinDirection(P0_16, 0);
	GPIO_PinDirection(P0_17, 0);

	Init_UART();
	
	
	
  ID_tache1 = osThreadCreate (osThread (tache1), NULL) ;
	
	
  osKernelStart ();                         // start thread execution 
	osDelay (osWaitForever) ;
	return 0;
}
