/*
 * dht11.c
 *
 *  Created on: Oct 11, 2022
 *      Author: b1d0
 */

//dht11 use single bus to communicate to microcontrollers

//libraries
#include "dht11.h"
#include "main.h"

//variables
//8bit integral RH + 8bit decimal RH + 8bit integral T + 8bit decimal T + 8bit checksum
extern uint8_t RH1, RH2, T1, T2;
extern uint16_t SUM, RH, TEMP;
extern float Temperature;
extern float Humidity;
extern uint8_t Presence;

/*
 * This part is too important the use micro delay and I add this function in main.c
 * so, you can add this function in your main.c file.
 * void Micro_Delay (uint16_t us) //Us mean MicroSecond
{
	HAL_TIM_SET_COUNTER(&htim7, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim7))<us);
	//Creating Micro_Delay to get under the millisecond data
}
*/

//Single Bus communication port
#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_1

/*If we can choose 2 GPIO pin, we can directly configurate but there is only one bus to communication
 * so we need to use input and output in the same pin
 */
void DHT11_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) //GPIO Pin selected in defines part
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) //GPIO Pin selected in defines part
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}


/*MCU sends start signal at least 18 milliseconds with staying GND
 *After MCU Checking response at least 20 to 40 microseconds with staying VCC
*/
void DHT11_Start (void)
{
	DHT11_Pin_Output (DHT11_PORT, DHT11_PIN);  // set the pin as output
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	Micro_Delay(18000);   // wait for 18ms
    HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1);   // pull the pin high
    Micro_Delay(20);   // wait for 20us
	DHT11_Pin_Input(DHT11_PORT, DHT11_PIN);    // set as input
}

/*
 *DHT sending response signal for 80 microseconds with staying GND
 *After DHT pulls up voltage and staying 80 microseconds with staying VCC
 */
uint8_t DHT11_Check_Response (void)
{
	uint8_t Response = 0;
	Micro_Delay(40);
	if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
	{
		Micro_Delay(80);
		if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
		else Response = -1; // 255
	}
	while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go low

	return Response;
}

/*If DHT sends "0" data, it will take 26-28 microseconds and if sends "1" data, it will take 70 microseconds.
 *So in the start of VCC which is sending data starting point of DHT sensor, we can check before the
 *40 microseconds to see is it finish to sending data it means it's "0" data, or it isn't sending data yet
 *it means it's "1" data.
 */
uint8_t DHT11_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high
		Micro_Delay(40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));  // wait for the pin to go low
	}
	return i;
}

void DHT11_Result (void)
{
	DHT11_Start();
  	Presence = DHT11_Check_Response();
	RH1 = DHT11_Read ();
	RH2 = DHT11_Read ();
	T1 = DHT11_Read ();
	T2 = DHT11_Read ();
	SUM = DHT11_Read();
}

/*
 *To see to result, use this:
 *TEMP = T1;
 *RH = RH1;
 *Temperature = (float) TEMP;
 *Humidity = (float) RH;
 *and also put this in the HAL_TIM_Base_Start(&htim7); in the USER CODE BEGIN 2 before the while(1);
 */
