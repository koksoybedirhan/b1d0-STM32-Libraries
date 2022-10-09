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

void MPU6050_Init (void);

float MPU6050_Temperature(void);

float MPU6050_Read_Accel_X (void);

float MPU6050_Read_Accel_Y (void);

float MPU6050_Read_Accel_Z (void);

float MPU6050_Read_Gyro_X (void);

float MPU6050_Read_Gyro_Y (void);

float MPU6050_Read_Gyro_Z (void);

double MPU6050_Kalman_Accel_X (double Accel_X_U);

double MPU6050_Kalman_Accel_Y (double Accel_Y_U);

double MPU6050_Kalman_Accel_Z (double Accel_Z_U);

double MPU6050_Kalman_Gyro_X (double Gyro_X_U);

double MPU6050_Kalman_Gyro_Y (double Gyro_Y_U);

double MPU6050_Kalman_Gyro_Z (double Gyro_Z_U);

float MPU6050_Kalman_Temp(float Temp_U);

#endif /* INC_MPU6050_H_ */
