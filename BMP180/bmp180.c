/*
 * bmp180.c
 *
 *  Created on: Sep 13, 2022
 *      Author: b1d0
 */

#include "stm32f4xx_hal.h" //This library for working with HAL function
#include "math.h" //This library for pow function

extern I2C_HandleTypeDef hi2c1; //Calling I2C
#define bmp180_i2c &hi2c1 //This define for changeable i2c
#define BMP180_ADDRESS (0x77<<1) //For 7 bits are 0x77 and last bit is W/R bit, so we have to shift one more and it has to be 0xEE

//Register Defines
//******************************
//bmp180_get_cal_param
short AC1 = 0;
short AC2 = 0;
short AC3 = 0;
unsigned short AC4 = 0;
unsigned short AC5 = 0;
unsigned short AC6 = 0;
short B1 = 0;
short B2 = 0;
short MB = 0;
short MC = 0;
short MD = 0;

//bmp_180_get_ut
long UT = 0;
short oss = 0;

//bmp180_get_up
long UP = 0;

//bmp180_get_temperature
long X1 = 0;
long X2 = 0;
long X3 = 0;
long B5 = 0;

//bmp180_calpressure
long B3 = 0;
unsigned long B4 = 0;
long B6 = 0;
unsigned long B7 = 0;

//Defines
long Temp = 0;
long Press = 0;
long atmPress = 101235;
HAL_StatusTypeDef status; //This define checking device ready or not
uint8_t callib_array[22] = {0}; //Calibration datas are 11*2 and it's for writing register in stm32 memory
uint16_t callib_first; //It's starting with AC1 (0xAA, 0xAB)
uint8_t temp_register;
uint8_t temp_arr[2] = {0};
uint8_t press_arr[3] = {0};
uint8_t press_register;

void BMP180_Test(void)
{
	HAL_StatusTypeDef status;
	status = HAL_I2C_IsDeviceReady(bmp180_i2c, BMP180_ADDRESS, 1, 50);
	if(HAL_OK == status)
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET);
	}
}

void BMP180_Callibration (void)
{
	callib_first = 0xAA;
	HAL_I2C_Mem_Read(bmp180_i2c, BMP180_ADDRESS, callib_first, 1, callib_array,22, HAL_MAX_DELAY); //Writing Callibration datas in stm32 memory

	AC1 = ((callib_array[0] << 8) | callib_array[1]);
	AC2 = ((callib_array[2] << 8) | callib_array[3]);
	AC3 = ((callib_array[4] << 8) | callib_array[5]);
	AC4 = ((callib_array[6] << 8) | callib_array[7]);
	AC5 = ((callib_array[8] << 8) | callib_array[9]);
	AC6 = ((callib_array[10] << 8) | callib_array[11]);
	B1 = ((callib_array[12] << 8) | callib_array[13]);
	B2 = ((callib_array[14] << 8) | callib_array[15]);
	MB = ((callib_array[16] << 8) | callib_array[17]);
	MC = ((callib_array[18] << 8) | callib_array[19]);
	MD = ((callib_array[20] << 8) | callib_array[21]);
}

void BMP180_Start (void)
{
	BMP180_Callibration(); //It's calling calibration function.
}


uint16_t Uncompensated_Temp (void) //Getting Uncompensated Temperature with information from data sheet.
{
	temp_register = 0x2E;
	HAL_I2C_Mem_Write(bmp180_i2c, BMP180_ADDRESS, 0xF4, 1, &temp_register, 1, 1000);
	HAL_Delay (5);  // wait 4.5 ms (data sheet recomended)
	HAL_I2C_Mem_Read(bmp180_i2c, BMP180_ADDRESS, 0xF6, 1, temp_arr, 2, 1000);
	return ((temp_arr[0]<<8) + temp_arr[1]);
}

uint32_t Uncompensated_Press (int oss) //Getting Uncompensated Pressure with information from data sheet.
{
	press_register = 0x34+(oss<<6);
	HAL_I2C_Mem_Write(bmp180_i2c, BMP180_ADDRESS, 0xF4, 1, &press_register, 1, 1000);
	/*oss means oversampling_setting
	 * Oversampling has three mode. First mode is ultra low power and conversion time 4.5 ms max
	 * Second mode is standart and conversion time 7.5 ms. Third mode is high resolution and conversion time is 13.5 ms.
	 * Forth mode is the ultra high resolution and conversion time 25.5 ms.
	 * If you want to work with highest resolution, you have to choose 4th but if you want to be more faster, you have to choose 1st.
	 */
	switch (oss)
	{
		case (0):
			HAL_Delay (5);
			break;
		case (1):
			HAL_Delay (8);
			break;
		case (2):
			HAL_Delay (14);
			break;
		case (3):
			HAL_Delay (26);
			break;
	}
	HAL_I2C_Mem_Read(bmp180_i2c, BMP180_ADDRESS, 0xF6, 1, press_arr, 3, 1000);
	return (((press_arr[0]<<16)+(press_arr[1]<<8)+press_arr[2]) >> (8-oss));
}

float BMP180_Calc_Temp (void) //Calculating temperature with information from data sheet.
{
	UT = Uncompensated_Temp();
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	Temp = (B5+8)/(pow(2,4));
	return Temp/10.0;
}


float BMP180_Calc_Press (int oss) //Calculating pressure with information from data sheet.
{
	UP = Uncompensated_Press(oss);
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	B6 = B5-4000;
	X1 = (B2 * (B6*B6/(pow(2,12))))/(pow(2,11));
	X2 = AC2*B6/(pow(2,11));
	X3 = X1+X2;
	B3 = (((AC1*4+X3)<<oss)+2)/4;
	X1 = AC3*B6/pow(2,13);
	X2 = (B1 * (B6*B6/(pow(2,12))))/(pow(2,16));
	X3 = ((X1+X2)+2)/pow(2,2);
	B4 = AC4*(unsigned long)(X3+32768)/(pow(2,15));
	B7 = ((unsigned long)UP-B3)*(50000>>oss);
	if (B7<0x80000000) Press = (B7*2)/B4;
	else Press = (B7/B4)*2;
	X1 = (Press/(pow(2,8)))*(Press/(pow(2,8)));
	X1 = (X1*3038)/(pow(2,16));
	X2 = (-7357*Press)/(pow(2,16));
	Press = Press + (X1+X2+3791)/(pow(2,4));
	return Press;
}


float BMP180_Calc_Alt (int oss)
{
	BMP180_Calc_Press (oss);
	return 44330*(1-(pow((Press/(float)atmPress), 0.19029495718)));
}

