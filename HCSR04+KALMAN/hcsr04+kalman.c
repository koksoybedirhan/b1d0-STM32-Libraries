/*
 * hcsr04.c
 *
 *  Created on: Oct 16, 2022
 *      Author: b1d0
 */

#include "hcsr04.h"
extern TIM_HandleTypeDef htim1;
#define timer &htim1

/*
 * HAL_TIM_Base_Start(&htim1);
 * HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
 * Add this after USER CODE BEGIN 2. It's near to while(1).
 */

//defines
#define TRIG_PIN GPIO_PIN_2
#define TRIG_PORT GPIOD
#define ECHO_PIN GPIO_PIN_1
#define ECHO_PORT GPIOD

//Variable
uint32_t pMillis;
uint32_t Value1 = 0;
uint32_t Value2 = 0;
uint16_t Distance  = 0;

//Kalman Variables
static const double R = 40;
static const double H = 1.00;
static double Q = 10;
static double P = 0;
static double U_hat = 0;
static double K = 0;

void Micro_Delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < time);
}

float HCSR04_Distance()
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	Micro_Delay(10);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low

    pMillis = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
    // wait for the echo pin to go high
    while (!(HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis + 10 >  HAL_GetTick());
    Value1 = __HAL_TIM_GET_COUNTER (timer);

    pMillis = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
    // wait for the echo pin to go low
    while ((HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis + 50 > HAL_GetTick());
    Value2 = __HAL_TIM_GET_COUNTER (timer);

    Distance = (Value2-Value1)* 0.034/2;
    return Distance;
}

float HCSR04_Kalman_Dist(double U)
{
	K = P * H / (H * P * H + R);
	U_hat = U_hat + K * (U - H * U_hat);
	P = (1 - K * H) * P + Q;
	return U_hat;
}

