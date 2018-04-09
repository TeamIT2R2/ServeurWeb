/*------------------------------------------------------------------------------
 * Serveur Web embarqué - Script CGI
 * GE2 parcours IT2R - IUT de Cachan
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: Serveur web HTTP
 * Modif: le 31/01/2016 pour la Team IT2R
 *----------------------------------------------------------------------------*/
 
#include <stdio.h>
#include <string.h>
#include "rl_net.h"
#include "Board_GLCD.h"
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */


uint8_t ip_addr[NET_ADDR_IP4_LEN];
uint8_t mask[NET_ADDR_IP4_LEN];
uint8_t gateway[NET_ADDR_IP4_LEN];
uint8_t pri_dns[NET_ADDR_IP4_LEN];
uint8_t sec_dns[NET_ADDR_IP4_LEN];

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern osThreadId ID_ClignoDroit;
extern osThreadId ID_ClignoGauche;
extern osThreadId ID_UART;


char lcd_text[20+1];
char Phares, Droit, Gauche;


// Fonction de gestions des requetes de type POST.
// \param[in]     code          type de données à gérer :
//                              - 0 = www-url-encoded form data,
//                              - sinon = autre (hors programme)
// \param[in]     data          pointeur sur donnee POST
// \param[in]     len           longueur donnee POST.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {
  char var[40];
	
	Phares=0;
	Droit=0;
	Gauche=0;
	osSignalSet(ID_UART,0x0001);
	
	if (code != 0) {
    // Les autres codes sont ignorés
    return;
  }
	
  if (len == 0) {
    // Si pas de donnée à traiter
    return;
  }

//TRAITEMENT DE LA CHAINE ENVOYE PAR LE NAVIGATEUR		
  do {
    // Appel de la fonction d'analyse de la trame "POST"
    data = netCGI_GetEnvVar (data, var, sizeof (var));	// var contient la donnée à gérer
		
		if (var[0] != 0) {  
			// si il y a une donnée à gérer

//Gestion des phares			
			if((strncmp(var,"led0=1",6))==0) Phares=1;
			
// Gestion du clignotant droit			
			if ((strncmp(var, "cliD=1",6))==0) Droit=1;
						
// Gestion du clignotant gauche			
			if ((strncmp(var, "cliG=1",6))==0) Gauche=1;
	
			
		//GPIO_PinWrite (0, 26, Phares);		
    }		
  } while (data);			// Tant qu'il a a des données à traiter
	
	LED_SetOut(Phares);
	if (Droit==1) osSignalSet(ID_ClignoDroit,0x0001);
	if (Gauche==1) osSignalSet(ID_ClignoGauche,0x0001);

}
 
// Fonction de génération des lignes CGI à mettre à jour
// \param[in]     env           environment string.
// \param[out]    buf           output data buffer.
// \return        number of bytes written to output buffer.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buf_len, uint32_t *pcgi) {
  uint32_t len = 0;
	int32_t adv;

  switch (env[0]) {
    
    case 'a':
      // Mise a jour du champ du script CGI
				len = sprintf (buf, &env[2], lcd_text);
				break;
		
		case 'b':
				ADC_StartConversion();
				adv = ADC_GetValue();
				len = sprintf (buf, &env[2], adv, (float) adv*3.3f/4096);
				break;
		
		case 'd':
				if (Phares==1)
				{
					len = sprintf(buf,&env[4],"checked");
				}
				else len = sprintf(buf,&env[4],"");
				break;
			
		case 'e':
				if (Droit==1)
				{
					len = sprintf(buf,&env[4],"checked");
				}
				else len = sprintf(buf,&env[4],"");
				break;
			
			
		case 'f':
				if (Gauche==1)
				{
					len = sprintf(buf,&env[4],"checked");
				}
				else len = sprintf(buf,&env[4],"");
				break;
    }
  return (len);
}