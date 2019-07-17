/*
 * @file util.c
 *
 * @date Feb 8, 2017
 * @author Santos
 */

#include "util.h"

/*!
 * \fn ConvUI2String(unsigned int j, char * s)
 * \brief Converte em ASCII o inteiro sem sinal j.
 * \param[in]  j valor inteiro sem sinal
 * \param[out] s representa&ccedil;&atilde;o em ASCII
 */
void ConvUI2String (unsigned int j, char * s) {
	short k = 0;
	char aux[11];
	/*!
	 * Passo 1: extrai os algarismos decimais
	 */
	if (j == 0) { ///< Caso especial
		aux[k] = 0;
		k++;
	}
	while (j) {
		aux[k] = j % 10;
		j = j / 10;
		k++;
	}
	/*!
	 * Passo 2: representa em ASCII os d&iacute;gitos decimais
	 */
	while (k > 0) {
		*s = (aux[k-1] + 0x30);
		s++;                    ///< incrementa o endere&ccedil;o por unidade do tipo de dados
		k--;
	}
	*s = 0;                     ///< adiciona o terminador '\0'
}

/*!
 * \fn ConvF2String(float j, char * s)
 * \brief Converte em ASCII um valor em ponto flutuante j com uma casa decimal.
 * \param[in]  j valor em ponto flutuante
 * \param[out] s representa&ccedil;&atilde;o em ASCII com uma casa decimal
 */
void ConvF2String (float j, char * s) {
	int i,g,f,vetor[5];
		i=0;
		g=(int)(j*10);// pegando somente a parte inteira

		if(g<10){ //caso especial em que o j eh inferior a 1
				*s=0x30;//para mudar para ascii
				s++;
			}
		
		while(g>0){
			vetor[i]=g % 10;//resto da divisao por 10
			g = g/10;
			i++;
			f=1; //flag para se caso entrar nessa funcao
		}
		i--;//eh necessario subtrair um de i ,pois assim que o mesmo sai do while incrementa-se um gerando um problema na hora de acessa-lo
		while(i>0){
			*s = (vetor[i]+0x30);
			s++;
			i--;
		}
		if(f==1)//verificando a flag
		{
		*s = '.' ;
		s++;
		*s = (vetor[0]+0x30);
		}
		else
		{
			*s = '.' ;
			s++;
			*s = 0x30;
			
		}
		s++;
		*s = 0;
}

/*!
 * \fn ConvString2UI(char * s, unsigned int *j)
 * \brief Converte uma string de caracteres num valor inteiro sem sinal j.
 * \param[in] s representa&ccedil;&atilde;o em ASCII com uma casa decimal
 * \param[out]  j valor inteiro sem sinal
 */
void ConvString2UI (char * s, unsigned int *j) {
	
	*j = 0;
	/*!
	 * Assume-se que a string s s&oacute; contem d&iacute;gitos v&aacute;lidos
	 */
	/*!
	 * Converte a parte inteira
	 */
	while (*s != '\0') {
		*j = (*j)*10 + (*s - 0x30);
		s++;
	}
}

/*!
 * \fn ConvString2F(char * s, float *j)
 * \brief Converte uma string de caracteres num valor em ponto flutuante j.
 * \param[in] s representa&ccedil;&atilde;o em ASCII com uma casa decimal
 * \param[out]  j valor em ponto flutuante
 */
void ConvString2F (char * s, float *j) {
	*j=0;
	while (*s != '.'){
		*j = (*j)*10 + (*s - 0x30);
		s++;
	}
	s++;
	*j = *j + ((*s-0x30)/10);
}

/*!
 * \fn ConvString2HHMMSS(char * s, uint8_t *horas, uint8_t *minutos, float *segundos)
 * \brief Converte uma string de caracteres em horas, minutos e segundos
 * \param[in] s string no formato HH:MM:SS.s
 * \param[out]  horas horas
 * \param[out]  minutos minutos
 * \param[out]  segundos segundos
 */
void ConvString2HHMMSS (char * s, uint8_t *horas, uint8_t *minutos, float *segundos) {
	char vetor[10];
	unsigned int aux;
	int i;
	i=0;
	while(*s != ':'){
		vetor[i] = *s;
		s++;
		i++;
	}
	s++;
	//i++;
	vetor[i]='\0';
	ConvString2UI(vetor,&aux);
	*horas = aux;
	i=0;
	while(*s != ':'){
			vetor[i] = *s;
			s++;
			i++;
		}
		s++;
		//i++;
		vetor[i]='\0';
		ConvString2UI(vetor,&aux);
		*minutos = aux;
		i=0;
		while(*s != '\0'){//pode-se passar tudo incluse o ponto pois na funcao de ConvString2F considera-se a virgula e resolve-se o problema
					vetor[i] = *s;
					s++;
					i++;
				}	
		
	ConvString2F(vetor,segundos);
}


/*!
 * \fn ConvMMSS2String(uint8_t MM, float SS, char * s)
 * \brief Converte no formato MM:SS.S em ASCII o valor em minutos e o valor em segundos.
 * \param[in]  MM valor em minutos
 * \param[in]  SS valor em segundos
 * \param[out] s representa&ccedil;&atilde;o em ASCII com uma casa decimal em segundos
 * 
 */
void ConvMMSS2String(uint8_t MM, float SS, char *s) {
	unsigned int p;
	float f;
	f = SS;
	p = (unsigned int)MM;
	if(p==0){//caso especial em que for 00 em MM
		*s=0x30;
		s++;
		*s=0x30;
		s++;
		*s=':';
		s++;
	}
	else if(p<10) {
		*s=0x30;
		s++;
		ConvUI2String(p, s);
		s++;
		*s=':';
		s++;
	}
	else {
		ConvUI2String(p, s);
		s=s+2;
		*s=':';
		s++;
	}
	if(f<10) {
		*s=0x30;//coloca-se zero na primeira posicao
		s++;
		ConvF2String (f, s);
		s = s + 3;//pois no caso esta se escrevendo 3 caracteres
	}
	else {
		ConvF2String (f, s);
		s = s + 4;//pois no caso se escreve 4 carcateres de uma so vez
		
	}
	//s++;
	*s = '\0';
}
void ConvHHMMSS2String (char * s, uint8_t horas, uint8_t minutos, float segundos) {
	unsigned int aux;//solucionar o problema com as variaveis
	if(horas<10) {
			*s=0x30;
			s++;
			aux= horas;
			ConvUI2String(aux, s);
			s++;
			*s=':';
			s++;
		}
		else {
			aux = horas;
			ConvUI2String(aux, s);
			s=s+2;
			*s=':';
			s++;
		}
	ConvMMSS2String(minutos,segundos,s);
	
}
