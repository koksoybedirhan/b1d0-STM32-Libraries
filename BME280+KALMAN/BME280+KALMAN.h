/*
 * bmp180.c
 *
 *  Created on: Oct 13, 2022
 *      Author: b1d0
 */

#ifndef INC_BME180_H_
#define INC_BME180_H_

#include "stm32f4xx_hal.h"

// Oversampling definitions
#define OSRS_OFF    	0x00
#define OSRS_1      	0x01
#define OSRS_2      	0x02
#define OSRS_4      	0x03
#define OSRS_8      	0x04
#define OSRS_16     	0x05

// MODE Definitions
#define MODE_SLEEP      0x00
#define MODE_FORCED     0x01
#define MODE_NORMAL     0x03

// Standby Time
#define T_SB_0p5    	0x00
#define T_SB_62p5   	0x01
#define T_SB_125    	0x02
#define T_SB_250    	0x03
#define T_SB_500    	0x04
#define T_SB_1000   	0x05
#define T_SB_10     	0x06
#define T_SB_20     	0x07

// IIR Filter Coefficients
#define IIR_OFF     	0x00
#define IIR_2       	0x01
#define IIR_4       	0x02
#define IIR_8       	0x03
#define IIR_16      	0x04

// REGISTERS DEFINITIONS
#define ID_REG      	0xD0
#define RESET_REG  		0xE0
#define CTRL_HUM_REG    0xF2
#define STATUS_REG      0xF3
#define CTRL_MEAS_REG   0xF4
#define CONFIG_REG      0xF5
#define PRESS_MSB_REG   0xF7

int BME280_Config (uint8_t osrs_t, uint8_t osrs_p, uint8_t osrs_h, uint8_t mode, uint8_t t_sb, uint8_t filter);

// Read the Trimming parameters saved in the NVM ROM of the device
void TrimRead(void);

/* To be used when doing the force measurement
 * the Device need to be put in forced mode every time the measurement is needed
 */
void BME280_WakeUP(void);

double BME280_Pressure (void);

double BME280_Humidity (void);

double BME280_Temperature (void);

double BME280_Kalman_Press(double U1);

double BME280_Kalman_Hum(double U2);

double BME280_Kalman_Temp(double U3);

#endif /* INC_BMP180_H_ */
