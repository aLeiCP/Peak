#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "ADS8517.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "HMI.h"
#include "stdio.h"
#include "math.h"
//#include "string.h"

#define PGA10  PAout(12)
#define PGA100 PAout(11)
#define FREQ   PCin(10)

#define Cover_Screen 			0x0000	//封面
#define Detection_Screen	0x0001	//测量画面
#define Config_Screen			0x0002	//配置画面
#define AVG_Screen				0x0003	//平均值画面
#define RMS_Screen				0x0004	//均方根画面
#define Sprt_Screen				0x0005	//有效值画面
#define MMP_Screen				0x0006	//峰峰值画面

#define AVG_ID						0x0001	//平均值电压值显示ID
#define RMS_ID						0x0002	//均方根电压值显示ID
#define Sprt_ID						0x0003	//有效值电压值显示ID
#define MMP_ID						0x0004	//峰峰值电压值显示ID
#define Y_Axe_ID					0x0006	//Y轴坐标数据显示起始ID

#define Full_Offset_ID		0x0001	//AD误差值显示ID
#define Zero_Offset_ID		0x0002	//零偏值显示ID
#define DC_Cycle_ID	  		0x0003	//测试周期显示ID
#define Ratio_ID					0x0011	//分压比显示ID
#define Curve_ID  				0x0069	//曲线显示ID
#define Zero_Key_ID				0x0016	//零压校准ID
#define Full_Key_ID				0x0004	//满度校准ID

#define Channel_num				0x0000		//曲线通道数


#define AC	0x00
#define DC	0x01
#define In_Yes 0x01		
#define In_No	 0x00
#define Res_Sample 660000.
#define Res_Total  10060000.//660000.//10060000
#define Min_InVol	0.5
#define ScanCycle 128
//#define USE_STDPERIPH_DRIVER   //定义使用库函数编程

#endif
