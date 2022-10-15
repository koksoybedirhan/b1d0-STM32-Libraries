/*
 * hcsr04.h
 *
 *  Created on: Oct 16, 2022
 *      Author: b1d0
 */

#ifndef INC_HCSR04_H_
#define INC_HCSR04_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

float HCSR04_Distance();

float HCSR04_Kalman_Dist();

#endif /* INC_HCSR04_H_ */
