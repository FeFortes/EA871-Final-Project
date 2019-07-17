/*
 * \file estrutura.c
 *
 * \date Feb 27, 2017
 * \author Santos
 */
#include <stdlib.h>
#include "estrutura.h"

void bufCircular_cria (buffer_circular *c, unsigned int n)
{
	c->head = 0;
	c->tail = 0;
	c->num = n;
	if (n == 0)
		c->elementos = NULL;
	else
		c->elementos = (char *)malloc(sizeof(char)*n);
	return;
}

uint8_t bufCircular_insere (buffer_circular *c, char v)
{

	unsigned int ind = (c->head+1)%c->num;
	if (ind == c->tail) return 0;
	 c->elementos[c->head] = v;
	 c->head = ind;
	 return 1;
}

char bufCircular_remove (buffer_circular *c)
{
	int v;
	if (c->head != c->tail) { //condicional se o comeco for diferente do final
	v = c->elementos[c->tail];//v recebe o elemento do final
	c->tail = (c->tail+1)%c->num;
	if (c->head == c->tail) c->head = c->tail = 0;//caso especial em que o comeco eh igual ao final
	return v;
	 } else {
		 return 0xff; ///< invalid value
	 }
}

char bufCircular_ultimo (buffer_circular *c)
{
	if (c->head != c->tail) {
	 return(c->elementos[c->tail]);
	} else {
		 return 0xff; ///< invalid value
	}	
}

char bufCircular_primeiro (buffer_circular *c)
{
	if (c->head != c->tail) {
	if (c->head == 0) return (c->elementos[c->num-1]);
	 return(c->elementos[c->head-1]);
	} else {
		 return 0xff; ///< invalid value
	}

}

uint8_t bufCircular_cheia (buffer_circular *c)
{
	if ((c->head+1)%c->num == c->tail) return 1;
	return 0;
}

uint8_t bufCircular_vazia (buffer_circular *c)
{
	if (c->head == c->tail) return 1;
	return 0;
}

unsigned int bufCircular_tamanho (buffer_circular *c) 
{
     return (c->num);
}

void bufCircular_deleta (buffer_circular *c)
{
     if (c->elementos) 
     	 free((void *)c->elementos);
     c->head = c->tail = c->num =0;
     c->elementos = NULL;
}
