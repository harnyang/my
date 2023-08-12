/******
	************************************************************************
	******
	** @project : XDrive_Step
	** @brief   : 具有多功能接口和闭环功能的步进电机
	** @author  : unlir (知不知啊)
	** @contacts: QQ.1354077136
	******
	** @address : https://github.com/unlir/XDrive
	******
	** @issuer  : REIN ( 知驭 实验室) (QQ: 857046846)             (discuss)
	** @issuer  : IVES (艾维斯实验室) (QQ: 557214000)             (discuss)
	** @issuer  : X_Drive_Develop     (QQ: Contact Administrator) (develop)
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
  ** @file     : Location_Interp.c/h
  ** @brief    : 位置插补器
  ** @versions : 1.1.7
  ** @time     : 2019/12/06
  ** @reviser  : unli (JiXi XuanCheng China)
  ** @explain  : null
*****/

//Oneself
#include "Location_Interp.h"

//Control
#include "control_config.h"

/****************************************  位置补插器  ****************************************/
/****************************************  位置补插器  ****************************************/
/****************************************  位置补插器  ****************************************/
//Location_Interp实例
Location_Interp_Typedef location_interp;

/**
  * 位置补插器初始化
  * @param  interp	位置插补器实例
  * @retval NULL
**/
void Location_Interp_Init(void)
{
	//源信号数据
	location_interp.record_location = 0;
	location_interp.record_location_last = 0;
	location_interp.est_location = 0;
	location_interp.est_speed_mut = 0;
	location_interp.est_speed = 0;
	//输出跟踪控制量
	location_interp.go_location = 0;
	location_interp.go_speed = 0;
}

/**
  * 位置补插器开始新任务
  * @param  interp			位置插补器实例
  * @param  real_location	实时位置
  * @param  real_speed		实时速度
  * @retval NULL
**/
void Location_Interp_NewTask(int32_t real_location, int32_t real_speed)//_将实时位置和速度赋值给记录位置、估计位置和速度//_根据实时位置和速度获取记录位置和初始估计速度和位置
{
	//更新源信号数据
	location_interp.record_location = real_location;//_这里real_location指的是编码器读取的位置通过形参est_location传入。
	location_interp.record_location_last = real_location;
	location_interp.est_location = real_location;
	location_interp.est_speed = real_speed;
}

/**
  * 位置补插器获得立即位置和立即速度
  * @param  interp			位置插补器实例
  * @param  goal_location	目标位置
  * @param  NULL
  * @retval NULL
**/
void Location_Interp_Capture_Goal(int32_t goal_location)//_根据目标位置获得，立即位置和立即速度、中间还使用最后一次的记录位置//_形参是goal_location但是前面过程已经将目标位置更新为最新的编码器读到的位置
{
	//记录源信号
	location_interp.record_location_last = location_interp.record_location;//_实时和目标之间的差值进行插补
	location_interp.record_location = goal_location;

	//估计源信号速度  
	location_interp.est_speed_mut += (	((location_interp.record_location - location_interp.record_location_last) * CONTROL_FREQ_HZ)//_两次位移差*频率=源信号的速度。
							  + ((int32_t)(location_interp.est_speed  << 6) - (int32_t)(location_interp.est_speed))
							 );
	location_interp.est_speed 	   = (location_interp.est_speed_mut >> 6);							//(对64取整)(向0取整)(保留符号位)//_这里是对速度先右移每次使用估计速度的时候再进行左移。
	location_interp.est_speed_mut  = (location_interp.est_speed_mut - (location_interp.est_speed << 6));	//(对64取余)(向0取整)(保留符号位) //_正常书减去一个已经64取整的数然后再扩大了的数
	//估计源信号位置
	location_interp.est_location = location_interp.record_location;

	//输出
	location_interp.go_location = location_interp.est_location;
	location_interp.go_speed = location_interp.est_speed;
}



