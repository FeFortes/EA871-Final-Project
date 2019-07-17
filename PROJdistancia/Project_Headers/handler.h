/*
 * handler.h
 *
 *  Created on: Aug 7, 2017
 *      Author: Ting
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include "util.h"
#include "port.h"

void ADC0_IRQHandler (void);

void LPTimer_IRQHandler (void);

void FTM2_IRQHandler(void);

void FTM0_IRQHandler(void);

void FTM1_IRQHandler(void);

#endif /* HANDLER_H_ */
