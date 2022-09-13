/*
 * bmp180.h
 *
 *  Created on: Sep 13, 2022
 *      Author: b1d0
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_

#include "stm32f4xx_hal.h"

void BMP180_Test(void); //This function for testing device working

void BMP180_Start (void); //This function writing bmp180 register data in memory

float BMP180_Calc_Temp (void); //This function give the calculating temperature

float BMP180_Calc_Press(int oss); //This function give the calculating pressure

float BMP180_Calc_Alt (int oss); //This function give the calculating altitude

#endif /* INC_BMP180_H_ */
