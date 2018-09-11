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

#define Cover_Screen 			0x0000	//����
#define Detection_Screen	0x0001	//��������
#define Config_Screen			0x0002	//���û���
#define AVG_Screen				0x0003	//ƽ��ֵ����
#define RMS_Screen				0x0004	//����������
#define Sprt_Screen				0x0005	//��Чֵ����
#define MMP_Screen				0x0006	//���ֵ����

#define AVG_ID						0x0001	//ƽ��ֵ��ѹֵ��ʾID
#define RMS_ID						0x0002	//��������ѹֵ��ʾID
#define Sprt_ID						0x0003	//��Чֵ��ѹֵ��ʾID
#define MMP_ID						0x0004	//���ֵ��ѹֵ��ʾID
#define Y_Axe_ID					0x0006	//Y������������ʾ��ʼID

#define Full_Offset_ID		0x0001	//AD���ֵ��ʾID
#define Zero_Offset_ID		0x0002	//��ƫֵ��ʾID
#define DC_Cycle_ID	  		0x0003	//����������ʾID
#define Ratio_ID					0x0011	//��ѹ����ʾID
#define Curve_ID  				0x0069	//������ʾID
#define Zero_Key_ID				0x0016	//��ѹУ׼ID
#define Full_Key_ID				0x0004	//����У׼ID

#define Channel_num				0x0000		//����ͨ����


#define AC	0x00
#define DC	0x01
#define In_Yes 0x01		
#define In_No	 0x00
#define Res_Sample 660000.
#define Res_Total  10060000.//660000.//10060000
#define Min_InVol	0.5
#define ScanCycle 128
//#define USE_STDPERIPH_DRIVER   //����ʹ�ÿ⺯�����

#endif
