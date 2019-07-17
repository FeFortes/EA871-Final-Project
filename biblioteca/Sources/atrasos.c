/*!
 * @file atrasos.c
 * @brief Gerencia os atrasos por la&ccedil;o de espera
 * @date Feb 7, 2017
 * @author Santos
 */

/*!
 * @brief gera um atraso correspondente a i itera&ccedil;&otilde;es
 * @param i n&uacute;mero de itera&ccedil;&otilde;es
 */
/*void delay( unsigned int i) 
{
	*while (i) i--;        
	*! Decrementa on&uacute;mero de itera&ccedil;&otilde;es at&eacute; zero *
}*/ 
/*nao se utiliza mais essa funcao delay*/

/*!
 * @brief gera um atraso correspondente a m&uacute;ltiplos de 10us
 * @param i n&uacute;mero de vezes
 */
void delay10us (unsigned int i) 
{
	asm( 
		"loop:;"
		"movs r1,#68;"
		"loop0:;"
		"sub r1,#1;"
		"bne loop0;"
		"sub r0,#1;"
		"bne loop;"
		);

}

