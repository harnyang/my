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


/*****
  ** @file     : hw_elec.c/h
  ** @brief    : 硬件电流控制器
  ** @versions : 2.1.2
  ** @time     : 2020/08/08
  ** @reviser  : unli (HeFei China)
  ** @explain  : null
*****/

//Oneself
#include "hw_elec.h"

//Base_Math
#include "sin_form.h"

//GPIO输出
#define Out_AP_H()		(HW_ELEC_AP_GPIO_Port -> BSRR = HW_ELEC_AP_Pin)//_pin5
#define Out_AP_L()		(HW_ELEC_AP_GPIO_Port -> BRR  = HW_ELEC_AP_Pin)
#define Out_AM_H()		(HW_ELEC_AM_GPIO_Port -> BSRR = HW_ELEC_AM_Pin)//_pin4
#define Out_AM_L()		(HW_ELEC_AM_GPIO_Port -> BRR  = HW_ELEC_AM_Pin)
#define Out_BP_H()		(HW_ELEC_BP_GPIO_Port -> BSRR = HW_ELEC_BP_Pin)//_pin3
#define Out_BP_L()		(HW_ELEC_BP_GPIO_Port -> BRR  = HW_ELEC_BP_Pin)
#define Out_BM_H()		(HW_ELEC_BM_GPIO_Port -> BSRR = HW_ELEC_BM_Pin)//_pin2
#define Out_BM_L()		(HW_ELEC_BM_GPIO_Port -> BRR  = HW_ELEC_BM_Pin)
//TIM输出
#define Out_PWMtoDAC_A(value)		(__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_APWM_CHANNEL, value))
#define Out_PWMtoDAC_B(value)		(__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_BPWM_CHANNEL, value))
//_pwm调波的比较输出。

//硬件电流实例
Coil_Typedef		coil_a;	//电流控制
Coil_Typedef		coil_b;	//电流控制

/**
  * @brief  12位基准电压混合输出
  * @param  elec_va: 电流通道a基准
  * @param  elec_vb: 电流通道b基准
  * @retval NULL
  */
void CurrentControl_VREF_12Bit_MixOut(uint16_t elec_va, uint16_t elec_vb)
{
	Out_PWMtoDAC_A((elec_va >> 2));
	Out_PWMtoDAC_B((elec_vb >> 2));
}

/**
  * @brief  硬件电流控制初始化
  * @param  NULL
  * @retval NULL
**/
void REIN_HW_Elec_Init(void)
{
	//外设配置
	REIN_GPIO_HwElec_Init();
	REIN_TIM_HwElec_Init();
}

/**
  * @brief  设置睡眠
  * @param  NULL
  * @retval NULL
  */
void REIN_HW_Elec_SetSleep(void)
{
	coil_a.dac_reg = 0;
	coil_b.dac_reg = 0;
	CurrentControl_VREF_12Bit_MixOut(coil_a.dac_reg, coil_b.dac_reg);
	Out_AP_L();	Out_AM_L();
	Out_BP_L();	Out_BM_L();
}

/**
  * @brief  设置驱动刹车
  * @param  NULL
  * @retval NULL
  */
void REIN_HW_Elec_SetBrake(void)
{
	coil_a.dac_reg = 0;
	coil_b.dac_reg = 0;
	CurrentControl_VREF_12Bit_MixOut(coil_a.dac_reg, coil_b.dac_reg);
	Out_AP_H();	Out_AM_H();
	Out_BP_H();	Out_BM_H();
}

/**
  * @brief  设置输出细分电流//_每个细分执行一次
  * @param  divide:  细分 (0 ~ 细分数)
  * @param  dac_reg: 电流 (0 ~ 3300mA)
  * @retval NULL
  */
void REIN_HW_Elec_SetDivideElec(uint32_t divide, int32_t elec_ma)
{
	//由细分数获得数组指针
	coil_b.conver = (divide) & (0x000003FF);//对1024取余//_因为下面的sin_pi_m2数组大小是1024大小的
	coil_a.conver = (coil_b.conver + (256)) & (0x000003FF);	//对1024取余//_这里加256是因为a的输出比b的相位提前90度
	
	//由数据指针获得整形数据(空间换时间方案)
	coil_a.sin_data = sin_pi_m2[coil_a.conver];//_根据输出位置获得对应的 Sin换算数值
	coil_b.sin_data = sin_pi_m2[coil_b.conver];

	//由整形数据获得DAC寄存器数据
	uint32_t dac_reg = abs(elec_ma);//电压电流关系为1:1(检流电阻为0.1欧)//_这里的电机驱动芯片为TB67H450
	dac_reg = (uint32_t)(dac_reg * 5083) >> 12; 		//(dac_reg * 4095 / 3300)的变种//?对数值的乘 //_DAC的分辨率是12位因为DAC为12位所以需要*4096
	dac_reg = dac_reg & (0x00000FFF);								//(对4096取余)(向小取整)(舍弃符号位)//_DAC数值为12位//?为什么是12位

	coil_a.dac_reg = (uint32_t)(dac_reg * abs(coil_a.sin_data)) >> sin_pi_m2_dpiybit;	//(--- / sin_1024_dpiy)的变种//_控制电压幅度的大小最大为4096/4096最小为0/4096
	coil_b.dac_reg = (uint32_t)(dac_reg * abs(coil_b.sin_data)) >> sin_pi_m2_dpiybit;	//(--- / sin_1024_dpiy)的变种//_即电压幅度的比例。
	//_这里对输出进行补偿：
	

	//DAC输出
	CurrentControl_VREF_12Bit_MixOut(coil_a.dac_reg, coil_b.dac_reg);//右移两位是因为pwm定时器比较寄存器的值为1024只有10位所以需要右移两位

	if(coil_a.sin_data > 0)				{	Out_AP_H();		Out_AM_L();		}
	else if(coil_a.sin_data < 0)	{	Out_AP_L();		Out_AM_H();		}
	else 													{	Out_AP_H();		Out_AM_H();		}
	if(coil_b.sin_data > 0)				{	Out_BP_H();		Out_BM_L();		}
	else if(coil_b.sin_data < 0)	{	Out_BP_L();		Out_BM_H();		}
	else													{	Out_BP_H();		Out_BM_H();		}	
		

/* 
	适配新驱动的输出函数。
*/
//本次输出略有一些改动经过推理确认是合理的方式，主要是在向上计数模式下面当输出高电平为有效时，低管是低电平，只要改变高管的高电平时间从而改变相电压的大小，
//在高电平为有效的模式下。小于计数值的为高电平而所以依据比较寄存器的值能够反映出电压的变化。在低电平有效的模式下，小于计数器的输出的是低电平，而另一个管是高电平，
//这时候低电平的变化反应了电压的变化。
	if (coil_a.sin_data > 0)
	{
		//(htim2.Instance->CR1 = (htim2.Instance->CR2) & (~TIM_CR1_DIR));//向上计数，DIR位为0

		(htim2.Instance->CCER = (htim2.Instance->CCER) || (TIM_CCER_CC4P));//有效电平为高电平模式
		coil_a.dac_reg = (uint32_t)(dac_reg * abs(coil_a.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_APWM_CHANNEL, coil_a.dac_reg>>2);
	}
	else // (coil_a.sin_data < 0)
	{
		//(htim2.Instance->CR1 = (htim2.Instance->CR2) || (TIM_CR1_DIR));//向下计数，DIR位为1

		(htim2.Instance->CCER = (htim2.Instance->CCER) & (~TIM_CCER_CC4P));//有效电平为低电平模式
		coil_a.dac_reg = (uint32_t)(dac_reg * abs(coil_a.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_APWM_CHANNEL, coil_a.dac_reg>>2);
	}
	if (coil_b.sin_data > 0)
	{
		//(htim2.Instance->CR1 = (htim2.Instance->CR2) & (~TIM_CR1_DIR));
/* 	向上计数，设置DIR位为0，输出寄存器的值位0时，是向上计数模式此时大于比较值的输出低电平，
	小于比较值输出的是高电平，由于从0开始计数，所以输出重点放在高电平上随着正弦曲线的变化高电平在最高点达到最大。 */

		(htim2.Instance->CCER = (htim2.Instance->CCER) || (TIM_CCER_CC4P));//有效电平为高电平模式
		coil_b.dac_reg = (uint32_t)(dac_reg * abs(coil_b.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_BPWM_CHANNEL, coil_b.dac_reg>>2);//寄存器直接输出，根据coil_b.dac_reg 的值来设置CCR寄存器的值。
	}
	else
	{
		//(htim2.Instance->CR1 = (htim2.Instance->CR2) || (TIM_CR1_DIR));
/* 	向下计数，DIR位为1，输出寄存器的值为1时，是向下计数模式此时大于比较值的输出低电平，
	小于比较值输出的是高电平，此时pwm的波形情况是低电平为主，低电平脉宽逐渐变宽。 */

		(htim2.Instance->CCER = (htim2.Instance->CCER) & (~TIM_CCER_CC4P));//有效电平为低电平模式
		coil_b.dac_reg = (uint32_t)(dac_reg * abs(coil_b.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_BPWM_CHANNEL, coil_b.dac_reg>>2);
	}

	if(coil_a.sin_data > 0)				{	Out_AP_L();	}//对应的是AN-
	else /* (coil_a.sin_data < 0) */	{	Out_AP_H();	}
	if(coil_b.sin_data > 0)				{	Out_BP_L();	}//对应的是AN-
	else /* (coil_b.sin_data < 0) */	{	Out_BP_H();	}



/* 
	适配新驱动的输出函数。
*/
//主要是在向上计数模式下面当输出高电平为有效时，低管是低电平，只要改变高管的高电平时间从而改变相电压的大小，
//在高电平有效的模式下。小于计数值的为高电平,另一个引脚输出的是低电平。而所以依据比较寄存器的值能够反映出电压的变化。
//在低电平有效的模式下，小于计数器的输出的是低电平，而另一个引脚是高电平，
//这时候低电平的变化反应了电压的变化。
	if (coil_a.sin_data > 0)
	{
		(htim2.Instance->CCER = (htim2.Instance->CCER) || (TIM_CCER_CC4P));//有效电平为高电平模式
		coil_a.dac_reg = (uint32_t)(dac_reg * abs(coil_a.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_APWM_CHANNEL, coil_a.dac_reg>>2);
	}
	else // (coil_a.sin_data <= 0)
	{
		(htim2.Instance->CCER = (htim2.Instance->CCER) & (~TIM_CCER_CC4P));//有效电平为低电平模式
		coil_a.dac_reg = (uint32_t)(dac_reg * abs(coil_a.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_APWM_CHANNEL, coil_a.dac_reg>>2);
	}
	if (coil_b.sin_data > 0)
	{
		(htim2.Instance->CCER = (htim2.Instance->CCER) || (TIM_CCER_CC4P));//有效电平为高电平模式
		coil_b.dac_reg = (uint32_t)(dac_reg * abs(coil_b.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_BPWM_CHANNEL, coil_b.dac_reg>>2);//寄存器直接输出，根据coil_b.dac_reg 的值来设置CCR寄存器的值。
	}
	else
	{
		(htim2.Instance->CCER = (htim2.Instance->CCER) & (~TIM_CCER_CC4P));//有效电平为低电平模式
		coil_b.dac_reg = (uint32_t)(dac_reg * abs(coil_b.sin_data)) >> sin_pi_m2_dpiybit;
		__HAL_TIM_SET_COMPARE(&HW_ELEC_PWM_Get_HTIM, HW_ELEC_BPWM_CHANNEL, coil_b.dac_reg>>2);
	}

	if(coil_a.sin_data > 0)				{	Out_AP_L();	}//对应的是AN-
	else /* (coil_a.sin_data <= 0) */	{	Out_AP_H();	}
	if(coil_b.sin_data > 0)				{	Out_BP_L();	}//对应的是BN-
	else /* (coil_b.sin_data <= 0) */	{	Out_BP_H();	}

}
