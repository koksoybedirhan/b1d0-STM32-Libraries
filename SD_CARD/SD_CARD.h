/*
 * sd.h
 *
 *  Created on: 29 Oct 2022
 *      Author: b1d0
 */

#ifndef INC_SD_H_
#define INC_SD_H_

#include "stm32f4xx_hal.h"
#include "fatfs.h"

void SD_Init(void);

void SD_Read(void);

void SD_Write(void);

#endif /* INC_SD_H_ */

