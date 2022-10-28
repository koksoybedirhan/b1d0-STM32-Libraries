/*
 * sd.c
 *
 *  Created on: 29 Oct 2022
 *      Author: b1d0
 */

#include "sd.h"

FATFS   	fs;
FRESULT 	res;
FIL 		file;
char buffer[100];
char write_buffer[50] = {"STM32 writing data in SD Card"};
char brw;

void SD_Init(void)
{
	res = f_mount(&fs, "", 0);
	if(res != FR_OK)
	{
		while(1);
	}
}

void SD_Read(void)
{
	res = f_open(&file, "test.txt", FA_READ);
	if(res == FR_OK)
	{
		res = f_read(&file, buffer, 25, &brw);
		if(res == FR_OK)
		{
			__NOP();
		}
		else
		{
			__NOP();
		}
	}
	else
	{
		__NOP();
	}
	f_close(&file);
}


void SD_Write(void)
{
	res = f_open(&file, "write.txt", FA_CREATE_ALWAYS  | FA_WRITE);
	if(res == FR_OK)
	{
		res = f_write(&file, write_buffer, sizeof(write_buffer), &brw);
		if(res == FR_OK)
		{
			__NOP();
		}
		else
		{
			__NOP();
		}
	}
	f_close(&file);
}

//Firstly Use SD_Init() in main.c, after you can use whatever you want.
