/*
 * mpu6050.c
 *
 *  Created on: Oct 8, 2022
 *      Author: b1d0
 */

#include "mpu6050.h"

extern I2C_HandleTypeDef hi2c1;

//Register Definitions
#define MPU6050_ADDR 0xD0
#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

//Variables
float Ax, Ay, Az, Gx, Gy, Gz, Accel_X, Accel_Y, Accel_Z, Gyro_X, Gyro_Y, Gyro_Z;
int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;
int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;

//Kalman Definitions
static const double R = 40; // noise coavirance (normally 10)
static const double H = 1.00; //measurment map scalar
static double Q = 10; //initial estimated covariance
static double P = 0; //initial error covariance (it must be 0)
static double K = 0; //initial kalman gain

void MPU6050_Init (void)
{
	uint8_t check;
	uint8_t Data;

	// check device ID WHO_AM_I

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR,WHO_AM_I_REG,1, &check, 1, 1000);

	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
	{
		// power management register 0X6B we should write all 0's to wake the sensor up
		Data = 0;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1,&Data, 1, 1000);

		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
		Data = 0x07;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ± 2g
		Data = 0x00;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ± 250 °/s
		Data = 0x00;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);
	}

}

void MPU6050_Accel_Config(void)
{
	uint8_t Rec_Data[6];
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
}

float MPU6050_Read_Accel_X(void)
{
	MPU6050_Accel_Config();
	Ax = Accel_X_RAW/16384.0;
	return Ax;
}

float MPU6050_Read_Accel_Y (void)
{
	MPU6050_Accel_Config();
	Ay = Accel_Y_RAW/16384.0;
	return Ay;
}

float MPU6050_Read_Accel_Z (void)
{
	MPU6050_Accel_Config();
	Az = Accel_Z_RAW/16384.0;
	return Az;
}

void MPU6050_Gyro_Config(void)
{
	uint8_t Rec_Data[6];
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
}

float MPU6050_Read_Gyro_X (void)
{
	MPU6050_Gyro_Config();
	Gx = Gyro_X_RAW/131.0;
	return Gx;
}

float MPU6050_Read_Gyro_Y (void)
{
	MPU6050_Gyro_Config();
	Gy = Gyro_Y_RAW/131.0;
	return Gy;
}

float MPU6050_Read_Gyro_Z (void)
{
	MPU6050_Gyro_Config();
	Gz = Gyro_Z_RAW/131.0;
	return Gz;
}

double MPU6050_Kalman_Accel_X (double Accel_X_U)
{
	Accel_X_U = MPU6050_Read_Accel_X();

	static double Accel_X_U_hat = 0; //initial estimated state

	K = P * H / (H * P * H + R);
	Accel_X_U_hat = Accel_X_U_hat + K * (Accel_X_U - H * Accel_X_U_hat);
	P = (1 - K * H) * P + Q;

	return Accel_X_U_hat;
}

double MPU6050_Kalman_Accel_Y (double Accel_Y_U)
{
	Accel_Y_U = MPU6050_Read_Accel_Y();

	static double Accel_Y_U_hat = 0; //initial estimated state

	K = P * H / (H * P * H + R);
	Accel_Y_U_hat = Accel_Y_U_hat + K * (Accel_Y_U - H * Accel_Y_U_hat);
	P = (1 - K * H) * P + Q;

	return Accel_Y_U_hat;
}

double MPU6050_Kalman_Accel_Z (double Accel_Z_U)
{
	Accel_Z_U = MPU6050_Read_Accel_Z();

	static double Accel_Z_U_hat = 0; //initial estimated state

	K = P * H / (H * P * H + R);
	Accel_Z_U_hat = Accel_Z_U_hat + K * (Accel_Z_U - H * Accel_Z_U_hat);
	P = (1 - K * H) * P + Q;

	return Accel_Z_U_hat;
}

double MPU6050_Kalman_Gyro_X (double Gyro_X_U)
{
	Gyro_X_U = MPU6050_Read_Accel_X();

	static double Gyro_X_U_hat = 0; //initial estimated state

	K = P * H / (H * P * H + R);
	Gyro_X_U_hat = Gyro_X_U_hat + K * (Gyro_X_U - H * Gyro_X_U_hat);
	P = (1 - K * H) * P + Q;

	return Gyro_X_U_hat;
}

double MPU6050_Kalman_Gyro_Y (double Gyro_Y_U)
{
	Gyro_Y_U = MPU6050_Read_Accel_Y();

	static double Gyro_Y_U_hat = 0; //initial estimated state

	K = P * H / (H * P * H + R);
	Gyro_Y_U_hat = Gyro_Y_U_hat + K * (Gyro_Y_U - H * Gyro_Y_U_hat);
	P = (1 - K * H) * P + Q;

	return Gyro_Y_U_hat;
}

double MPU6050_Kalman_Gyro_Z (double Gyro_Z_U)
{
	Gyro_Z_U = MPU6050_Read_Accel_Z();

	static double Gyro_Z_U_hat = 0; //initial estimated state

	K = P * H / (H * P * H + R);
	Gyro_Z_U_hat = Gyro_Z_U_hat + K * (Gyro_Z_U - H * Gyro_Z_U_hat);
	P = (1 - K * H) * P + Q;

	return Gyro_Z_U_hat;
}
