/*
 *
 * \file main.c
 * \brief Módulo TPM,ADC.
 * \mainpage Projeto final - medida distancia.
 * \details media a distancia e faz o buzzer tocar 
 * \author Fernando Fortes Granado e Rodrigo Diniz Junqueira Santos
 * \date 30/11/2017
 * \note Documenta&ccedil;&atilde;o do c&oacute;digo no formato de Doxygen:
 *       http://mcuoneclipse.com/2014/09/01/automatic-documentation-generation-doxygen-with-processor-expert/ 
 *
 *
 *
 */



#include "atrasos.h"
#include "port.h"
#include "derivative.h" /*! include peripheral declarations */
#include "util.h"
#include "lcdled.h"
#include "adc.h"
#include "nvic.h"
#include "lptmr.h"
#include "tpm.h"


#define MAX 0xfff               ///< maximum conversion extent (12 bits)
#define VREFH 3300 //3.3  2921   ///< tens&atilde;o de refer&ecirc;ncia alta (mV)
#define VREFL 0.0               ///< tens&atilde;o de refer&ecirc;ncia baixa (mV)
#define VTEMP25  703.125 //703.125     ///< tens&atilde;0 de refer&ecirc;ncia temperatura 25
#define COUNTER_OVF 1024
uint8_t flag_ADC0, flag_novo_tempo, ligar = 1;                ///< quantidade de elementos no buffer
unsigned short buffer2[2];         ///< buffer para dois canais

int ciclos, aux_buzzer = 0;
float tempoviagem;
int ciclos_desligado = 10/0.000003051;/*valor inicial para que nao se tenha lixo*/

/*!
 * \fn trigger (void)
 * \brief Envia sinal de trigger
 */
void trigger() {/*!funcao responsavel por mandar o trigger do HC-SR04*/
	GPIOE_PTOR = (1<<22);
	delay10us(2);
	GPIOE_PTOR = (1<<22);
}



int main(void)
{
//char vetorLED[9] = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00};
char vetorLED[9] = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};	
float valor ,temperatura,velocidadeAR,m;
char s[10];
	flag_novo_tempo = 0;
	ADCConfig config;
	uint8_t flag_print_dist = 0 ,i=0,j=0;
	float aux_dist_media,distancias_recentes[220];
	int indice5 = 0;
	/*! Pino que serve um canal do m&oacute;dulo ADC0 */
	initPort(B);
	setaMux(&PORTB_PCR1, 0b000);/*adc*/			///< fun&ccedil;&atildeo ADC0 - canal 9 

	
	initPort(E);
	setaMux(&PORT_PCR_REG(PORTE_BASE_PTR,21), 0b000); /*! buzzer - TPM - sera setado como TPM em conf_buzzer */
	setaMux(&PORT_PCR_REG(PORTE_BASE_PTR,22), 0b001); /*! trigger */
	setaMux(&PORT_PCR_REG(PORTE_BASE_PTR,23), 0b011); /*! eco */
	/*!setando como saida porta E*/
	
	GPIOE_PDDR = GPIOE_PDDR |(1<<22); /*! pino do trigger eh saida */
	
	initTPM (2, 0b011, 16, 0);
	initTPM (1, 0b011, COUNTER_OVF, 0);
	
	initTPM2Cn(1,0b0011,0); /*! input capture  -- capture on rising and falling edge*/
	initTPM1Cn(1,0b0101,0); /*! toggle output on match - output compare - buzzer*/
	/*!
	 * Inicializa LCD
	 */
	initLcdledGPIO();
	enviaLed(0x00);
	initLCD();

	/*!
	 * Inicializa LPTMR com interrup&ccedil;&atilde;o
	 * bypassed
	 * pr&eacute;-escala = /2
	 * fonte = LPO
	 * valor m&aacute;ximo de contagem de forma que a frequ&ecirc;ncia de amostragem seja 2/s
	 */
	initLPTMR0(1, 0b0000, 0b01, 250);/*LPO*/
	enableInterrupLPTMR0();

	/*! 
	 * Inicialize o conversor ADC com mecanismo de interrup&ccedil;&atilde;o 
	 * divisor do clock do n&uacute;cleo: 1
	 * diff: convers&atilde;o unipolar
	 * adlsmp: tempo de convers&atilde;o curto 
	 * adlsts: 20 extra ADCK ciclos; total 24 ADCK ciclos 
	 * mode: resolu&ccedil;&atilde;o da convers&atilde;o 12 bits
	 * adiclk: clock do barramento/2
	 * adiv: divisor de frequ&ecirc;ncia 8 
	 * muxsel: sele&ccedil;&atilde;o de canal: A 
	 * adco:  convers&atilde;o simples
	 * avge:  habilita m&eacute;dia das amostras
	 * avgs: 8 amostras por m&eacute;dia
	 * adtrg: disparo por software
	 * alternate: fontes alternativas desabilitadas
	 * pre: canal A
	 * fonte: LPTMR0
	 */ 
	config.diff = config.adlsmp = config.muxsel = 0;/**/
	config.adtrg = 0;
	config.alternate = 0;
	config.pre = 0;
	config.fonte = 0b1110; /*LPTMR0*/
	config.adco = 0;/**/
	config.adlsts = 0b00; 
	config.mode = 0b01; 
	config.adiclk = 0b01;            
	config.adiv = 0b11; 			 
	config.avge = 1; /**/
	config.avgs = 0b01; 

	/**/
	
	initADC0(0, &config);	
	enableInterrupADC0();
	
	
	/*!
	 * Habiliatr interrup&ccedil;&otilde;es no NVIC
	 */
	enableNVICLPTMR0(3);
	enableNVICADC0(3);
	
	enableNVICTPM2(3);
	enableTPM2CnInterrup(1);
	
	enableNVICTPM1(3); /*! modulo em que esta o buzzer TPM1_CH1 */
	enableTPM1CnInterrup(1); /*! TPM do buzzer */
	
	/*!
	 * Amostrar sinais em dois canais
	 * Baixar a bandeira de captura de estados e come&ccedil; com o canal 9
	 */	
	flag_ADC0 = 0;
	selecionaCanalADC0 (0x09);

	RS(0);
	enviaLCD(0b10000000, 4);
	mandaString("Distancia  [cm]");
	
	trigger();
		
	for(;;) {
		if (flag_ADC0) {/*com o intuito em se ter maior precisao na velocidade do ar medimos a temperatura por meio do adc*/
/*!
			 * Sensor de temperatura: buffer[1]
			 * Temperature Sensor for the HCS08 Microcontroller Family (document AN3031).
			 * http://cache.freescale.com/files/microcontrollers/doc/app_note/AN3031.pdf
			 * Assumindo V_{TEMP25} =  0.703125V
			 */
			valor = ((float)(buffer2[1])*((VREFH-VREFL)/MAX));      ///< valor em tens&atile;o
			if(valor >= 703.125)   m = 1.646;
			else  m = 1.769;
			temperatura = 25. - ((valor - VTEMP25)/1000.*m); /*28.4.8----->sensor de temperatura*/;                ///< valor em graus Celsius
			flag_ADC0 = 0; ///< baixar a bandeira
		}
		if(flag_novo_tempo) {
			velocidadeAR = 33130 + (60 * temperatura);/*velocidade do ar em cm*/  /* http://en.wikipedia.org/wiki/Speed_of_sound*/  ///olhar esta errado
			/*! ate chegar aqui ja vai ter lido uma temperatura. Nao vai ter lixo */
			distancias_recentes[i] = (velocidadeAR*tempoviagem)/2;/*medida em cm*/
			i++;
			if(i == 220) {
				aux_dist_media = 0;
				for(j=0; j<220; j++){ /*! soma todas as medidas de distancia */
					aux_dist_media = aux_dist_media + distancias_recentes[j];
				}
				aux_dist_media = aux_dist_media/220.; /*! faz a media */
				ciclos_desligado = (aux_dist_media/200)/0.000391;
				i=0;
				flag_print_dist = 1;
			}
			flag_novo_tempo = 0;
		}
		

		if(flag_print_dist) { /*! escreve a media das distancias recentes no LCD; apaga LEDs vermelhos conforme menor distancia */
			ConvF2String (aux_dist_media, s);
			RS(0);
			enviaLCD(0b11000000, 4);
			mandaString(s);
			flag_print_dist = 0;
			if(aux_dist_media < 100) {
				indice5 = (int)(aux_dist_media/12.5); /*! divide o intervalo de distancia de 0-100cm em 8 pedacos, de acordo com os 8 LEDs disponiveis0 */
				enviaLed(vetorLED[indice5]);
			}
			else
				enviaLed(0xFF);
	
		}
		
	}
	
	return 0;
}
