/*
 * mpu6050.h
 *
 *  Created on: Oct 8, 2022
 *      Author: b1d0
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include <math.h>

void MPU6050_Init (void);

float MPU6050_Temperature(void);

float MPU6050_Read_Accel_X (void);

float MPU6050_Read_Accel_Y (void);

float MPU6050_Read_Accel_Z (void);

float MPU6050_Read_Gyro_X (void);

float MPU6050_Read_Gyro_Y (void);

float MPU6050_Read_Gyro_Z (void);

float MPU6050_Roll_Angle (void);

float MPU6050_Kalman_Roll_Angle (void);

double MPU6050_Kalman_Accel_X (void);

double MPU6050_Kalman_Accel_Y (void);

double MPU6050_Kalman_Accel_Z (void);

double MPU6050_Kalman_Gyro_X (void);

double MPU6050_Kalman_Gyro_Y (void);

double MPU6050_Kalman_Gyro_Z (void);

float MPU6050_Kalman_Temp(void);

#endif /* INC_MPU6050_H_ */
