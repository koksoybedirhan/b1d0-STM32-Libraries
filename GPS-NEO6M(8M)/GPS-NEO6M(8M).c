/*
 * neo6m.c
 *
 *  Created on: 17 Oct 2022
 *      Author: b1d0
 */

//Libraries
#include "neo6m.h"

//Defines
extern UART_HandleTypeDef huart1;
extern lwgps_t gps;
#define uart &huart1

//Variables
uint8_t rx_data = 0;
uint8_t rx_buffer[128];
uint8_t rx_index = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart) {
		if(rx_data != '\n' && rx_index < sizeof(rx_buffer)) {
			rx_buffer[rx_index++] = rx_data;
		} else {
			lwgps_process(&gps, rx_buffer, rx_index+1);
			rx_index = 0;
			rx_data = 0;
		}
		HAL_UART_Receive_IT(uart, &rx_data, 1);
	}
}

/*
 * You have to add this defines private user code in main.c file
 * lwgps_t gps;
 * extern uint8_t rx_data[1];
 * Also you have to add this user code 2 before the while(1) in main.c
 * lwgps_init(&gps);
 * HAL_UART_Receive_IT(&huart1, &rx_data, 1);
 */

