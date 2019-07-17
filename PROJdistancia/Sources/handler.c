/*!
 * \file handler.c
 *
 * \date Nov 30, 2017
 * \author Fernando Fortes Granado e Rodrigo Diniz Junqueira Santos
 */
#include "derivative.h"
#include "handler.h"
#include "adc.h"
#include "tpm.h"
#include "port.h"
#include "atrasos.h"

extern short buffer2[];
extern uint8_t flag_ADC0, flag_novo_tempo, ligar;
extern int ciclos, ciclos_desligado;
extern float tempoviagem;

extern int aux_buzzer;

/*! 
 * \fn ADC0_IRQHandler (void)
 * \brief Rotina de servi&ccedil;o para atender ADC0 
 */
void ADC0_IRQHandler (void) {
	static uint8_t ind=0;
	
	if( ADC0_SC1A & ADC_SC1_COCO_MASK )
	{
		switch (ind) {
		case 0:
			  buffer2[ind] = ADC0_RA;   ///< ler o registrador do conversor (limpa COCO)
			  selecionaCanalADC0 (0x1A);
			  ind++;
			  break;
		case 1:
			  buffer2[ind] = ADC0_RA;   ///< ler o registrador do conversor (limpa COCO)
			  disableInterrupADC0 ();
			  ind = 0;
			  flag_ADC0 = 1;           ///< levanta a bandeira de ter dados prontos no ADC0
			  break;
		}
	}
}

/*!
 * \fn LPTMR0_IRQHandler (void)
 * \brief Rotina de servi&ccedil;o para atender LPTMR0
 */
void LPTimer_IRQHandler(void) {
	/*!
	 * Fa&ccedil;a a medida e habilite a interrup&ccedil;&atilde;o do ADC0
	 */
	selecionaCanalADC0 (0x09);
	enableInterrupADC0();
	
	LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;   ///< Limpar pend&ecirc;ncias (w1c)
}

/*! 
 * \fn FTM2_IRQHandler (void)
 * \brief Rotina de servi&ccedil;o para atender TPM2
 */
void FTM2_IRQHandler(void) {
	if ((TPM2_SC & (TPM_SC_TOF_MASK | TPM_SC_TOIE_MASK)) == (TPM_SC_TOF_MASK | TPM_SC_TOIE_MASK)) {
		ciclos++;
		TPM2_SC |= TPM_SC_TOF_MASK;     		///< limpa flag de interrupcao de overflow
	}

	if (TPM2_STATUS & TPM_STATUS_CH1F_MASK) {
		if (GPIOE_PDIR & GPIO_PIN(23)) { ///< borda de subida de PTE23		
			TPM2_SC = TPM2_SC | TPM_SC_TOIE_MASK; /*! habilita interrupcao no overflow */
			ciclos = 0;
		}
		else {//borda de descida de PTE23
			TPM2_SC &= ~(TPM_SC_TOIE_MASK); /*! desabilita interrupcao no overflow*/
			tempoviagem = ciclos*0.0000061;/*0.000003051*/
			flag_novo_tempo = 1;
			trigger();
		}
		TPM2_C1SC |= TPM_CnSC_CHF_MASK;     		///< CHF=TPM0_C2SC[7]: w1c (limpa a pend&ecirc;ncia do canal)
	}
}

/*! 
 * \fn FTM1_IRQHandler (void)
 * \brief Rotina de servi&ccedil;o para atender TPM1
 */

void FTM1_IRQHandler(void) {
	if (TPM1_STATUS & TPM_STATUS_CH1F_MASK) {
		aux_buzzer++; /*! variavel que incrementa para contar o tempo em que o buzzer ficara ligado e desligado */
		if (aux_buzzer >= ciclos_desligado) {
			if (ligar) {
				setaMux(&PORT_PCR_REG(PORTE_BASE_PTR,21), 0b011); /*!liga o buzzer*/
				ligar = 0;
			}
			else {
				setaMux(&PORT_PCR_REG(PORTE_BASE_PTR,21), 0b000);/*!desliga o buzzer*/
				ligar = 1;
			}
			aux_buzzer = 0;
			//atualizar tempo_desligado
		}
		
		
	}
	TPM1_C1SC |= TPM_CnSC_CHF_MASK;     		///< CHF=TPM0_C2SC[7]: w1c (limpa a pend&ecirc;ncia do canal)
}
