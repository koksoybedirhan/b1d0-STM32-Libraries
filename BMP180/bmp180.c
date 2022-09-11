/*
 * bmp180.c
 *
 *  Created on: Sep 12, 2022
 *      Author: Lenovo
 */

#include "bmp180.h"

extern I2C_HandleTypeDef hi2c1;

int BMP180_Test()
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_IsDeviceReady(&hi2c1, (0x77<<1), 1, 100);

	if(HAL_OK == status)
	{
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		HAL_Delay(500);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET);
	}
}
