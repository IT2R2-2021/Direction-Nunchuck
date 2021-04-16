#include "Board_LED.h"                  // ::Board Support:LED
#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "stdio.h"

#define SLAVE_I2C_ADDR       0x52

extern ARM_DRIVER_I2C Driver_I2C1; //"déclaration" structure I2C1
extern ARM_DRIVER_USART Driver_USART3;

uint8_t dataLue[10];

void Init_UART(void){
	Driver_USART3.Initialize(NULL);
	Driver_USART3.PowerControl(ARM_POWER_FULL);
	Driver_USART3.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART3.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART3.Control(ARM_USART_CONTROL_RX,1);
}

void Init_I2C(void){
	Driver_I2C1.Initialize(NULL);
	Driver_I2C1.PowerControl(ARM_POWER_FULL);
	Driver_I2C1.Control(	ARM_I2C_BUS_SPEED,				// 2nd argument = débit
							ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
//	Driver_I2C1.Control(	ARM_I2C_BUS_CLEAR,
		//					0 );
}

void write1byte(unsigned char composant, unsigned char registre, unsigned char valeur)
{
	uint8_t tab2[10];
	tab2[0] = registre;
	tab2[1] = valeur;
		// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
		Driver_I2C1.MasterTransmit (composant, tab2, 2, false);		// false = avec stop
		while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
	
}

void writeCmd(unsigned char composant, unsigned char cmd)
{

		// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
		Driver_I2C1.MasterTransmit (composant, &cmd, 1, false);		// false = avec stop
		while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
	
}

void readNBytes (unsigned char composant,unsigned int nb)
{

	 unsigned char registre;
	
	
		// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
		/*Driver_I2C1.MasterTransmit (composant, &registre, 0, true);		// false = avec stop
		while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission*/
	
		Driver_I2C1.MasterReceive (composant, dataLue, nb, false);		// false = avec stop
		while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
	

}

int main(void)
{
	char tab[50];
	LED_Initialize();
	Init_I2C();
	Init_UART();
	
	write1byte(0x52,0xF0,0x55);
	write1byte(0x52,0xFb,0x00);
//write1byte2(0x52,0x00,0x00);

	while (1)
  {
		
		//LED_On (1);
		writeCmd(0x52,0x00);
		readNBytes(0x52,6);	
		//LED_On (2);
		//LED_On (3);
		
		sprintf(tab,"x%03d%03d",dataLue[0],dataLue[1]);
		
		while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART3.Send(tab,7);

		
		if (dataLue[0]>0x88)
		{
			LED_On(3);
			LED_Off(1);
		//while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		//Driver_USART3.Send("droite  ",10);
		
		}
		else if (dataLue[0]<0x80)
		{
			LED_On(1);
			LED_Off(3);
		//while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		//Driver_USART3.Send("gauche  ",10);
	
		}
		else if ((dataLue[0]<=0x88)&&(dataLue[0]>=0x80))
		{
			LED_Off(1);
			LED_Off(3);
		//while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		//Driver_USART3.Send("neutreX ",10);
		}
		
		/*if (dataLue[1]>0x88)
		{
			LED_On(2);
			LED_Off(0);
		while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART3.Send("avant   \n\r",10);
		}
		else if (dataLue[1]<0x80)
		{
			LED_On(0);
			LED_Off(2);
		while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART3.Send("arriere \n\r",10);
		}
		else if ((dataLue[1]<=0x88)&&(dataLue[1]>=0x80))
		{
			LED_Off(2);
			LED_Off(0);
		while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART3.Send("neutreY \n\r",10);
		}*/
  }
return 0;	
}