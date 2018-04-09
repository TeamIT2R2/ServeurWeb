/*------------------------------------------------------------------------------
 * Serveur Web embarqué 
 * GE2 parcours IT2R - IUT de Cachan
 *------------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: Serveur web HTTP
 * Modif: le 31/01/2016 pour la Team IT2R
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_net.h"                     /* Network definitions                */

#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "Board_LED.h"
#include "Board_Buttons.h"
#include "Board_ADC.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART


void ClignoDroit (void const* argument);
void ClignoGauche (void const* argument);
void UART (void const* argument);
//void sendCommand(char * command, int tempo_ms);
void Init_UART(void);
//void Init_Maitre(void);


extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern char Droit;
extern char Gauche;
extern char Phares;
extern ARM_DRIVER_USART Driver_USART1;

osThreadId ID_ClignoDroit;
osThreadId ID_ClignoGauche;
osThreadId ID_UART;

osThreadDef (ClignoDroit,osPriorityNormal,1,0);
osThreadDef (ClignoGauche,osPriorityNormal,1,0);
osThreadDef (UART,osPriorityNormal,1,0);


/*----------------------------------------------------------------------------
  Thread principale : gestion réseau
 *---------------------------------------------------------------------------*/
int main (void) {
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	LED_Initialize     ();
  ADC_Initialize     ();
  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 1*24, "Mon premier serveur web");
	GLCD_DrawString         (0, 2*24, "IT2R Power !");
	Init_UART();
	
	netInitialize     ();
	
	ID_ClignoDroit = osThreadCreate(osThread (ClignoDroit), NULL); //Création de la tâche
	ID_ClignoGauche = osThreadCreate(osThread (ClignoGauche), NULL); //Création de la tâche
	ID_ClignoGauche = osThreadCreate(osThread (UART), NULL); //Création de la tâche
	
	osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
	return 0;
	
}


void ClignoDroit (void const* argument)
{
	while(1)
	{
		osSignalWait(0x0001,osWaitForever);
		while (Droit)
		{
			LED_On(4);
			osDelay(750);
			LED_Off(4);
			osDelay(750);
		}
	}
}

void ClignoGauche (void const* argument)
{
	while(1)
	{
		osSignalWait(0x0001,osWaitForever);
		while (Gauche)
		{
			LED_On(7);
			osDelay(750);
			LED_Off(7);
			osDelay(750);
		}
	}
}

void UART (void const* argument)
{
	int i;
	char donnee[3];
	donnee[0]=Phares;
	donnee[1]=Droit;
	donnee[2]=Gauche;
	
	while (1)
	{		
		osSignalWait(0x0001,osWaitForever);
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		
		for (i=0; i<3; i++)
		{
    Driver_USART1.Send(&donnee[i],1);
		Driver_USART1.Send("a",1);
		}
		
	}
}
	
void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

//void sendCommand(char * command, int tempo_ms)
//{
//	int len;
//	len = strlen (command);
//	Driver_USART1.Send(command,len); // send the read character to the esp8266
//	osSignalWait(0x02, osWaitForever);		// sommeil fin emission
//	osDelay(tempo_ms);		// attente traitement retour
//}


//void Init_Maitre(void)
//{
//	sendCommand("AT+RESET=1\r\n",2000);
//	sendCommand("AT+ROLE=1\r\n",2000);
//	sendCommand("AT+CMODE=1\r\n",2000);
//}
