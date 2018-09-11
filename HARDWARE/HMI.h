#ifndef __HMI_H
#define __HMI_H

#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "usart.h"
#include "delay.h"
#define HandShake_OK  0xFF
#define HandShake_NO  0x00
#define Rest_OK       0xFF
#define Rest_NO       0x00
#define Key_up				0x00
#define Key_down      0x01

extern u8 DACAI_Over[4];
extern u8 DACAI_TXData[256];
extern u8 DACAI_RXData[256];
extern u8 DACAI_Paremeter[256];
extern u8 DACAI_Head;
extern u16 DACAI_DataLen;
extern char Text_Buffer[256];
extern u8 Key_Buffer[16];
extern u16 Operate_Screen,Operate_Control;
extern u8 KeyUp,xx1;
extern u8 TextUp;
extern u8 TextLen;
extern u16 New_Screen,New_Control;

void DACAI_Init(u32 bound);
void DACAI_Send(void);
void DACAI_Read(void);
void Check_HMI(void);
void DACAI_ReadScreen(void);
void DACAI_Read_Text(void);
void DACAI_Read_Key(void);
void DACAI_Curve_DisToEnd(u16 Screen_ID,u16 Control_ID,u8 Channel,u16 Data_Len,u8 *Curve_Data);
void DACAI_Curve_Clean(u16 Screen_ID,u16 Control_ID,u8 Channel);
void DACAI_Text_UpData(u16 Screen_ID,u16 Control_ID,u8 Data_len,char *Text_Data);
void DACAI_Text_Clean(u16 Screen_ID,u16 Control_ID);
void DACAI_GoScreen(u16 Screen_ID);
void DACAI_TXRead_Text(u16 Screen_ID,u16 Control_ID);
void DACAI_Bezz(u8 Time10ms);

extern u8 DACAI_HandShake(void);
extern u8 DACAI_Rest(void);

#endif

