/*
 * bme280.c
 *
 *  Created on: Oct 13, 2022
 *      Author: b1d0
 */

#include "bme280.h"

extern I2C_HandleTypeDef hi2c1;
#define BME280_I2C &hi2c1

#define SUPPORT_64BIT 1
#define BME280_ADDRESS 0xEC //0x76<<1

extern float Temperature, Pressure, Humidity;

uint8_t chipID, TrimParam[36], trimdata[32];
uint16_t dig_T1, dig_P1, dig_H1, dig_H3;
int16_t  dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9, dig_H2,  dig_H4, dig_H5, dig_H6;
int32_t tRaw, pRaw, hRaw;

//Read the Trimming parameters saved in the NVM ROM of the device
//This function came from datasheet page 24
void TrimRead(void)
{
	// Read NVM from 0x88 to 0xA1
	HAL_I2C_Mem_Read(BME280_I2C, BME280_ADDRESS, 0x88, 1, trimdata, 25, HAL_MAX_DELAY);

	// Read NVM from 0xE1 to 0xE7
	HAL_I2C_Mem_Read(BME280_I2C, BME280_ADDRESS, 0xE1, 1, (uint8_t *)trimdata+25, 7, HAL_MAX_DELAY);

	// Arrange the data as per the datasheet (page no. 24)
	dig_T1 = (trimdata[1]<<8) | trimdata[0];
	dig_T2 = (trimdata[3]<<8) | trimdata[2];
	dig_T3 = (trimdata[5]<<8) | trimdata[4];
	dig_P1 = (trimdata[7]<<8) | trimdata[5];
	dig_P2 = (trimdata[9]<<8) | trimdata[6];
	dig_P3 = (trimdata[11]<<8) | trimdata[10];
	dig_P4 = (trimdata[13]<<8) | trimdata[12];
	dig_P5 = (trimdata[15]<<8) | trimdata[14];
	dig_P6 = (trimdata[17]<<8) | trimdata[16];
	dig_P7 = (trimdata[19]<<8) | trimdata[18];
	dig_P8 = (trimdata[21]<<8) | trimdata[20];
	dig_P9 = (trimdata[23]<<8) | trimdata[22];
	dig_H1 = trimdata[24];
	dig_H2 = (trimdata[26]<<8) | trimdata[25];
	dig_H3 = (trimdata[27]);
	dig_H4 = (trimdata[28]<<4) | (trimdata[29] & 0x0f);
	dig_H5 = (trimdata[30]<<4) | (trimdata[29]>>4);
	dig_H6 = (trimdata[31]);
}

int BME280_Config (uint8_t osrs_t, uint8_t osrs_p, uint8_t osrs_h, uint8_t mode, uint8_t t_sb, uint8_t filter)
{
	// Read the Trimming parameters
	TrimRead();

	uint8_t datatowrite = 0;
	uint8_t datacheck = 0;

	// Reset the device
	datatowrite = 0xB6;  // reset sequence
	if (HAL_I2C_Mem_Write(BME280_I2C, BME280_ADDRESS, RESET_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return -1;
	}
	HAL_Delay (100);

	// write the humidity oversampling to 0xF2
	datatowrite = osrs_h;
	if (HAL_I2C_Mem_Write(BME280_I2C, BME280_ADDRESS, CTRL_HUM_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return -1;
	}
	HAL_Delay (100);
	HAL_I2C_Mem_Read(BME280_I2C, BME280_ADDRESS, CTRL_HUM_REG, 1, &datacheck, 1, 1000);
	if (datacheck != datatowrite)
	{
		return -1;
	}

	// write the standby time and IIR filter coeff to 0xF5
	datatowrite = (t_sb <<5) |(filter << 2);
	if (HAL_I2C_Mem_Write(BME280_I2C, BME280_ADDRESS, CONFIG_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return -1;
	}
	HAL_Delay (100);
	HAL_I2C_Mem_Read(BME280_I2C, BME280_ADDRESS, CONFIG_REG, 1, &datacheck, 1, 1000);
	if (datacheck != datatowrite)
	{
		return -1;
	}
	// write the pressure and temp oversampling along with mode to 0xF4
	datatowrite = (osrs_t <<5) |(osrs_p << 2) | mode;
	if (HAL_I2C_Mem_Write(BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return -1;
	}
	HAL_Delay (100);
	HAL_I2C_Mem_Read(BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datacheck, 1, 1000);
	if (datacheck != datatowrite)
	{
		return -1;
	}
	return 0;
}

int BMEReadRaw(void)
{
	uint8_t RawData[8];

	// Check the chip ID before reading
	HAL_I2C_Mem_Read(&hi2c1, BME280_ADDRESS, ID_REG, 1, &chipID, 1, 1000);

	if (chipID == 0x60)
	{
		// Read the Registers 0xF7 to 0xFE
		HAL_I2C_Mem_Read(BME280_I2C, BME280_ADDRESS, PRESS_MSB_REG, 1, RawData, 8, HAL_MAX_DELAY);

		/* Calculate the Raw data for the parameters
		 * Here the Pressure and Temperature are in 20 bit format and humidity in 16 bit format
		 */
		pRaw = (RawData[0]<<12)|(RawData[1]<<4)|(RawData[2]>>4);
		tRaw = (RawData[3]<<12)|(RawData[4]<<4)|(RawData[5]>>4);
		hRaw = (RawData[6]<<8)|(RawData[7]);

		return 0;
	}
	else return -1;
}

/* To be used when doing the force measurement
 * the Device need to be put in forced mode every time the measurement is needed
 */
void BME280_WakeUP(void)
{
	uint8_t datatowrite = 0;

	// first read the register
	HAL_I2C_Mem_Read(BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datatowrite, 1, 1000);

	// modify the data with the forced mode
	datatowrite = datatowrite | MODE_FORCED;

	// write the new data to the register
	HAL_I2C_Mem_Write(BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datatowrite, 1, 1000);

	HAL_Delay (100);
}

int32_t t_fine;
int32_t BME280_compensate_T_int32(int32_t adc_T)
{
	int32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1)))>> 12) *((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

uint32_t BME280_compensate_P_int64(int32_t adc_P)
{
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	return (uint32_t)p;
}

/* Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
   Output value of â€œ47445â€ represents 47445/1024 = 46.333 %RH
*/
uint32_t bme280_compensate_H_int32(int32_t adc_H)
{
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) *\
			v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *\
					((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +\
							((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) +\
					8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *\
			((int32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t)(v_x1_u32r>>12);
}

//Pressure Measurment
double BME280_Pressure (void)
{
	if (BMEReadRaw() == 0)
	{
		  if (pRaw == 0x800000)
		  {
			  Pressure = 0; // value in case temp measurement was disabled
		  }
		  else
		  {
			  Pressure = (BME280_compensate_P_int64 (pRaw))/256.0;  // as per datasheet, the pressure is x256
		  }
	}
	else
	{
		Pressure = 0;
	}
	return Pressure;
}

//Humidity Measurment
double BME280_Humidity (void)
{
	if (BMEReadRaw() == 0)
	{
		if (hRaw == 0x8000)
		{
			Humidity = 0; // value in case temp measurement was disabled
		}
		else
		{
			Humidity = (bme280_compensate_H_int32 (hRaw))/1024.0;  // as per datasheet, the temp is x1024
		}
	}
	else
	{
		Humidity = 0;
	}

	return Humidity;
}

//Temperature Measurment
double BME280_Temperature(void)
{
	if (BMEReadRaw() == 0)
	{
		if (tRaw == 0x800000)
		{
			Temperature = 0; // value in case temp measurement was disabled
		}
		else
		{
			Temperature = (BME280_compensate_T_int32 (tRaw))/100.0;  // as per datasheet, the temp is x100
		}
	}
	else
	{
		Temperature = 0;
	}
	return Temperature;
}

double BME280_Kalman_Press(double U1)
{
	static const double R1 = 40; // noise coavirance (normally 10)
	static const double H1 = 1.00; //measurment map scalar
	static double Q1 = 10; //initial estimated covariance
	static double P1 = 0; //initial error covariance (it must be 0)
	static double U1_hat = 100000; //initial estimated state
	static double K1 = 0; //initial kalman gain

	U1 = BME280_Pressure();
	K1 = P1 * H1 / (H1 * P1 * H1 + R1);
	U1_hat = U1_hat + K1 * (U1 - H1 * U1_hat);
	P1 = (1 - K1 * H1) * P1 + Q1;
	return U1_hat;
}

double BME280_Kalman_Hum(double U2)
{
	static const double R2 = 40; // noise coavirance (normally 10)
	static const double H2 = 1.00; //measurment map scalar
	static double Q2 = 10; //initial estimated covariance
	static double P2 = 0; //initial error covariance (it must be 0)
	static double U2_hat = 50; //initial estimated state
	static double K2 = 0; //initial kalman gain

	U2 = BME280_Humidity();
	K2 = P2 * H2 / (H2 * P2 * H2 + R2);
	U2_hat = U2_hat + K2 * (U2 - H2 * U2_hat);
	P2 = (1 - K2 * H2) * P2 + Q2;
	return U2_hat;
}

double BME280_Kalman_Temp(double U3)
{
	static const double R3 = 40; // noise coavirance (normally 10)
	static const double H3 = 1.00; //measurment map scalar
	static double Q3 = 10; //initial estimated covariance
	static double P3 = 0; //initial error covariance (it must be 0)
	static double U3_hat = 25; //initial estimated state
	static double K3 = 0; //initial kalman gain

	U3 = BME280_Temperature();
	K3 = P3 * H3 / (H3 * P3 * H3 + R3);
	U3_hat = U3_hat + K3 * (U3 - H3 * U3_hat);
	P3 = (1 - K3 * H3) * P3 + Q3;
	return U3_hat;
}
