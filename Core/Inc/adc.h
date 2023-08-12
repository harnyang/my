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


#ifndef __ADC_H
#define __ADC_H

#ifdef __cplusplus
extern "C" {
#endif

//引用端口定义
#include "kernel_port.h"

extern uint16_t whole_adc_data[2][12];

void REIN_ADC_Init(void);		//REIN_ADC综合初始化
//_新添加的代码：
//_begin
#define COLLECT_BUFFER_SIZE 2

#define ADC_NU  ADC1
#define ADC_CURRENTA  ADC_CHANNEL_0
#define ADC_CURRENTB  ADC_CHANNEL_1
#define ADC_NU_CH_CLK_ENABLE()  do { __HAL_RCC_ADC1_CLK_ENABLE();}while(0)  

#define ADC_DMA_CH                      DMA1_Channel1
#define ADC_DMA_CH_IRQn                 DMA1_Channel1_IRQn
#define ADC_DMA_CH_IRQHandler           DMA1_Channel1_IRQHandler

void ADC_DMA_enable(uint16_t cndtr);

#define ADC_DMA_CHx_IS_TC()              ( DMA1->ISR & (1 << 1) )    /* 判断 DMA1_Channel1 传输完成标志, 这是一个假函数形式,
                                                                         * 不能当函数使用, 只能用在if等语句里面 
                                                                         */
#define ADC_DMA_CHx_CLR_TC()             do{ DMA1->IFCR |= 1 << 1; }while(0) /* 清除 DMA1_Channel1 传输完成标志 */


extern void collect_data_handing(void);
//_end
#ifdef __cplusplus
}
#endif

#endif
