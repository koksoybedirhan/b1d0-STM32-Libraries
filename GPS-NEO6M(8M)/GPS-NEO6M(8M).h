/*
 * neo6m.h
 *
 *  Created on: 17 Oct 2022
 *      Author: b1d0
 */

#ifndef INC_NEO6M_H_
#define INC_NEO6M_H_

//Libraries
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "lwgps/lwgps.h"

//Function
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_NEO6M_H_ */

