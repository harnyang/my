/******
	************************************************************************
	******
	** @project : XDrive_Step
	** @brief   : Stepper motor with multi-function interface and closed loop function. 
	** @brief   : 具有多功能接口和闭环功能的步进电机
	** @author  : unlir (知不知啊)
	** @contacts: QQ.1354077136
	******
	** @address : https://github.com/unlir/XDrive
	******
	************************************************************************
	******
	** {Stepper motor with multi-function interface and closed loop function.}
	** Copyright (c) {2020}  {unlir(知不知啊)}
	** 
	** This program is free software: you can redistribute it and/or modify
	** it under the terms of the GNU General Public License as published by
	** the Free Software Foundation, either version 3 of the License, or
	** (at your option) any later version.
	** 
	** This program is distributed in the hope that it will be useful,
	** but WITHOUT ANY WARRANTY; without even the implied warranty of
	** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	** GNU General Public License for more details.
	** 
	** You should have received a copy of the GNU General Public License
	** along with this program.  If not, see <http://www.gnu.org/licenses/>.
	******
	************************************************************************
******/


#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

//引用端口定义
#include "kernel_port.h"

/********** STALL_SIGNAL **********/
/********** STALL_SIGNAL **********/
/********** STALL_SIGNAL **********/
void REIN_GPIO_STALL_SIGNAL_Init(void);

/********** Button **********/
/********** Button **********/
/********** Button **********/
void REIN_GPIO_Button_Init(void);  	 //GPIO初始化(Button)

/********** HwElec **********/
/********** HwElec **********/
/********** HwElec **********/
void REIN_GPIO_HwElec_Init(void);   	//GPIO初始化(HwElec)

/********** MT6816 **********/
/********** MT6816 **********/
/********** MT6816 **********/
void REIN_GPIO_MT6816_ABZ_Init(void);  //GPIO初始化(MT6816_ABZ)
void REIN_GPIO_MT6816_SPI_Init(void);  //GPIO初始化(MT6916_SPI)

/********** Modbus **********/
/********** Modbus **********/
/********** Modbus **********/
void REIN_GPIO_Modbus_Init(void);			//GPIO初始化(Modbus)

/********** OLED **********/
/********** OLED **********/
/********** OLED **********/
void REIN_GPIO_OLED_Init(void);			 //GPIO初始化(OLED)

/********** SIGNAL **********/
/********** SIGNAL **********/
/********** SIGNAL **********/
void REIN_GPIO_SIGNAL_COUNT_Init(void);		//GPIO初始化(SIGNAL_COUNT)
void REIN_GPIO_SIGNAL_COUNT_DeInit(void);	//GPIO清理(SIGNAL_COUNT)
void REIN_GPIO_SIGNAL_PWM_Init(void);			//GPIO初始化(SIGNAL_PWM)
void REIN_GPIO_SIGNAL_PWM_DeInit(void);		//GPIO清理(SIGNAL_PWM)

#ifdef __cplusplus
}
#endif

#endif
