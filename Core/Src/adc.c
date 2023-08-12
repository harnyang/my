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

//Oneself
#include "adc.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
//ADC_HandleTypeDef hadc2;
//DMA_HandleTypeDef hdma_adc2;

//_uint16_t whole_adc_data[2][12];
uint16_t collect_data[COLLECT_BUFFER_SIZE];

/**
  * @brief  REIN_ADC综合初始化
  * @param  NULL
  * @retval NULL
**/
void REIN_ADC_Init(void)
{
#if(XDrive_Run == XDrive_REIN_Basic_H1_0)
	
	#error "undefined"

#elif (XDrive_Run == XDrive_REIN_Basic_H1_1)
//_begin
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_PeriphCLKInitTypeDef ADC_CLK_InitStruct = {0};
	ADC_ChannelConfTypeDef  ADC_CHANNEL_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	ADC_NU_CH_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();//通道11是在DMA1下

	//gpio初始化
	GPIO_InitStructure.Mode=GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pin=GPIO_PIN_0 | GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//adc的时钟配置
	ADC_CLK_InitStruct.AdcClockSelection=RCC_PERIPHCLK_ADC;//adc外设时钟
	ADC_CLK_InitStruct.PeriphClockSelection=RCC_ADCPCLK2_DIV6;//使用6分频 72/6=12
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLK_InitStruct);

	//adc的句柄配置
	hadc1.Instance=ADC_NU;//adc1
	hadc1.Init.ContinuousConvMode=ENABLE;//连续转换模式
	hadc1.Init.DataAlign=ADC_DATAALIGN_RIGHT;//数据右对齐
	hadc1.Init.DiscontinuousConvMode=DISABLE;//关闭间断模式
	hadc1.Init.ExternalTrigConv=ADC_SOFTWARE_START;//外部软件触发模式，如果使用外部触发可以设置触发源为ADC_ExternalTrigConv_T1_TRGO;而同样也需要设置定时器的触发输出
	//TIM3->CR2 &= ~(0x0070);									//清除定时器3MMS位
	//TIM3->CR2 |= 0x0020;									//选择定时器更新事件作为触发输出
	hadc1.Init.NbrOfConversion=2;//转换的通道数通道0和通道1
	hadc1.Init.NbrOfDiscConversion=0;//间断转换的通道数
	hadc1.Init.ScanConvMode=ADC_SCAN_ENABLE;//使能连续扫描模式因为有两个通道
	HAL_ADC_Init(&hadc1);

	//adc的通道设置
	ADC_CHANNEL_InitStruct.Channel=ADC_CHANNEL_0;///通道A优先
	ADC_CHANNEL_InitStruct.Rank=ADC_REGULAR_RANK_1;
	ADC_CHANNEL_InitStruct.SamplingTime=ADC_SAMPLETIME_1CYCLE_5;
	HAL_ADC_ConfigChannel(&hadc1,&ADC_CHANNEL_InitStruct);

	ADC_CHANNEL_InitStruct.Channel=ADC_CHANNEL_1;
	ADC_CHANNEL_InitStruct.Rank=ADC_REGULAR_RANK_2;
	ADC_CHANNEL_InitStruct.SamplingTime=ADC_SAMPLETIME_1CYCLE_5;
	HAL_ADC_ConfigChannel(&hadc1,&ADC_CHANNEL_InitStruct);
	//校准ADC
	HAL_ADCEx_Calibration_Start(&hadc1);       

	//DMA的句柄设置
	hdma_adc1.Instance=ADC_NU;
	hdma_adc1.Init.Direction=DMA_PERIPH_TO_MEMORY;
	hdma_adc1.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;
	hdma_adc1.Init.MemInc=DMA_MINC_ENABLE;//内存是增量模式；
	hdma_adc1.Init.Mode=DMA_NORMAL;
	hdma_adc1.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;
	hdma_adc1.Init.PeriphInc=DMA_PINC_DISABLE;
	hdma_adc1.Init.Priority=DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&hdma_adc1);

	__HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
	
	HAL_DMA_Start_IT(&hdma_adc1,(uint16_t)ADC_NU->DR,&collect_data,0);//这里不设定采集的数据长度，通过自定义的ADC_DMA_enable函数进行定义

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_ADC_Start_DMA(&hadc1,&collect_data, 0); //这里同上面一样

	ADC_DMA_enable(COLLECT_BUFFER_SIZE);//采集的数组的大小是2，每次采集一次数据，可更改次数使得输出更加准确

/* 
  //AFIO初始化 
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	//_gpio初始化
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//DMA初始化 
	// ADC1 Init 
	hdma_adc1.Instance = DMA1_Channel1;														//DMA通道1
	hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;							//外设到内存
	hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;									//外设地址(保持)
	hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;											//内存地址(递增)
	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;	//外设宽度(半字)
	hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;		//内存宽度(半字)
	hdma_adc1.Init.Mode = DMA_CIRCULAR;														//模式(循环)
	hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;							//传输优先级(最高)
	if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
	{
		Error_Handler();
	}
	__HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
	
  // ADC初始化 
  ADC_ChannelConfTypeDef sConfig = {0};
	//ADC初始化
	__HAL_RCC_ADC1_CLK_ENABLE();
  hadc1.Instance = ADC1;																	//ADC1
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;							//扫描模式
  hadc1.Init.ContinuousConvMode = ENABLE;									//连续转换(启用)
  hadc1.Init.DiscontinuousConvMode = DISABLE;							//失能连续转换(禁用)
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;				//触发转换(软件触发)
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;							//数据对齐(右对齐)
  hadc1.Init.NbrOfConversion = 2;													//转换次数(2)
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  //配置列队1
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  //配置列队2
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
	
	HAL_ADCEx_Calibration_Start(&hadc1);
	
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&whole_adc_data[0][0], 2);
	 */
#else

	#error "undefined"
	
#endif
}

void ADC_DMA_enable(uint16_t cndtr)
{
    ADC_NU->CR2 &= ~(1 << 0);                 /* 先关闭ADC */

    ADC_DMA_CH->CCR &= ~(1 << 0);           /* 关闭DMA传输 */
    while (ADC_DMA_CH->CCR & (1 << 0));     /* 确保DMA可以被设置 */
    ADC_DMA_CH->CNDTR = cndtr;              /* DMA传输数据量 */
    ADC_DMA_CH->CCR |= 1 << 0;              /* 开启DMA传输 */

    ADC_NU->CR2 |= 1 << 0;                    /* 重新启动ADC */
    ADC_NU->CR2 |= 1 << 22;                   /* 启动规则转换通道 */
}
//中断回调函数
void ADC_DMA_CH_IRQHandler()
{
	if (ADC_DMA_CHx_IS_TC())
    {
        //g_adc_dma_sta = 1;                      /* 标记DMA传输完成 */
		//这里调用采集处理函数对采集数据进行计算，并输出pwm：
		collect_data_handing();
		ADC_DMA_CHx_CLR_TC();                /* 清除DMA1 数据流7 传输完成中断 */
    }
}
