/*
 * dht11.h
 *
 *  Created on: Oct 11, 2022
 *      Author: b1d0
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "stm32f4xx_hal.h"

void DHT11_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin); //GPIO Pin selected in defines part

void DHT11_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin); //GPIO Pin selected in defines part

void DHT11_Start (void);

uint8_t DHT11_Check_Response (void);

uint8_t DHT11_Read (void);

void DHT_Result(void);

#endif /* INC_DHT11_H_ */
