#include "HMI.h"

#define DACAI_OK USART_RX_BUF[0]==0xEE&&USART_RX_BUF[USART_RX_STA-3]==0xFC&&USART_RX_BUF[USART_RX_STA-2]==0xFF&&USART_RX_BUF[USART_RX_STA-1]==0xFF

u8 DACAI_Over[4]={0xFF,0xFC,0xFF,0xFF};
u8 DACAI_TXData[256];
u8 DACAI_RXData[256];
u8 DACAI_Paremeter[256];
u8 DACAI_Head=0xEE;
u16 Operate_Screen,Operate_Control;
u16 New_Screen,New_Control;
u16 DACAI_DataLen;
char Text_Buffer[256];
u8 Key_Buffer[16];
u8 flag;
u8 KeyUp,xx1;
u8 TextUp;
u8 TextLen;
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)  
	{
		Res =USART_ReceiveData(USART1);
		if(USART_RX_STA==0&&Res==0xEE)	Flag=1;
		if(Flag==1)
		{
			USART_RX_BUF[USART_RX_STA]=Res;
			USART_RX_STA++;
			if(DACAI_OK)
				Flag=0;
		}
  } 
}

void DACAI_Init(u32 bound)
{
	uart_init(bound);
}
void DACAI_Send()
{
	u16 i;
	for(i=0;i<DACAI_DataLen;i++)
	{
		USART_SendData(USART1,DACAI_TXData[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
	
	for(i=0;i<DACAI_DataLen;i++)
		DACAI_TXData[i]=0x00;
	DACAI_DataLen=0;
}
void DACAI_Read(void)
{
	u8 x;
	if(USART_RX_STA>0)
	{
		if(DACAI_OK)
		{
			DACAI_DataLen=USART_RX_STA;
			for(x=0;x<USART_RX_STA;x++)
			{
				DACAI_RXData[x]=USART_RX_BUF[x];
				USART_RX_BUF[x]=0;
			}
			flag=1;
			USART_RX_STA=0;
		}
	}
}
u8 DACAI_HandShake(void)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0x04;
	for(i=0;i<4;i++)	
		DACAI_TXData[2+i]=DACAI_Over[i];
	DACAI_DataLen=6;
	DACAI_Send();
	i=0;
//	while(!USART_RX_STA&0x8000)
//	{
//		i++;
//		if(i==50000)
//			return 0x00;
//	}
//	DACAI_Read();
//	if(DACAI_RXData[1]==0x55)
//		return 0xff;
//	else
		return 0x00;
}

u8 DACAI_Rest(void)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0x07;
	DACAI_TXData[2]=0x35;
	DACAI_TXData[3]=0x5a;
	DACAI_TXData[4]=0x53;
	DACAI_TXData[5]=0xa5;
	for(i=0;i<4;i++)	
		DACAI_TXData[6+i]=DACAI_Over[i];
	DACAI_DataLen=10;
	DACAI_Send();
//	i=0;
//	while(!(USART_RX_STA&0x8000))
//	{
//		i++;
//		if(i==50000)
//			return 0x00;
//	}
//	
//	DACAI_Read();
//	if(DACAI_RXData[1]==0x55)
//		return 0xff;
//	else
		return 0x00;
	
}

void DACAI_ReadScreen(void)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0xB1;
	DACAI_TXData[2]=0x01;
	for(i=0;i<4;i++)	
		DACAI_TXData[3+i]=DACAI_Over[i];	
	DACAI_DataLen=3+4;
	DACAI_Send();
	while(!flag)
		DACAI_Read();
	Check_HMI();
}
void DACAI_Read_Text(void)
{
	u8 i;
	TextUp=1;
	TextLen=DACAI_DataLen-13;
	for(i=0;i<TextLen||DACAI_RXData[8+i]!=0x00;i++)
		Text_Buffer[i]=DACAI_RXData[8+i];
}
void DACAI_Read_Key(void)
{
	u8 i;	
	KeyUp=1;
	for(i=0;i<DACAI_DataLen-12;i++)
		Key_Buffer[i]=DACAI_RXData[8+i];
}
void Check_HMI(void)
{
	u8 x;
	DACAI_Read();
	if(flag==1)
	{
		flag=0;
		switch(DACAI_RXData[1])
		{
			case 0xB1:	
				switch(DACAI_RXData[2])
				{
					case 0x01:
						New_Screen=DACAI_RXData[3];
						New_Screen=(New_Screen<<8)|DACAI_RXData[4];
						break;
					case 0x11:
						New_Screen=Operate_Screen=DACAI_RXData[3];
						New_Screen=Operate_Screen=(Operate_Screen<<8)|DACAI_RXData[4];
						Operate_Control=DACAI_RXData[5];
						Operate_Control=(Operate_Control<<8)|DACAI_RXData[6];
						if(DACAI_RXData[7]==0x10)
							DACAI_Read_Key();
						if(DACAI_RXData[7]==0x11)
							DACAI_Read_Text();
						break;
				}
				break;
		}
		for(x=0;x<DACAI_DataLen;x++)
			DACAI_RXData[x]=0;
	}
}


void DACAI_Bezz(u8 Time10ms)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0x61;
	DACAI_TXData[2]=Time10ms;
	for(i=0;i<4;i++)	
		DACAI_TXData[3+i]=DACAI_Over[i];
	DACAI_DataLen=7;	
	DACAI_Send();
}

void DACAI_Curve_DisToEnd(u16 Screen_ID,u16 Control_ID,u8 Channel,u16 Data_Len,u8 *Curve_Data)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0xB1;
	DACAI_TXData[2]=0x32;
	DACAI_TXData[3]=Screen_ID>>8;
	DACAI_TXData[4]=Screen_ID&0x00ff;
	DACAI_TXData[5]=Control_ID>>8;
	DACAI_TXData[6]=Control_ID&0x00ff;
	DACAI_TXData[7]=Channel;
	DACAI_TXData[8]=Data_Len>>8;
	DACAI_TXData[9]=Data_Len&0x00ff;
	for(i=0;i<Data_Len;i++)
		DACAI_TXData[10+i]=Curve_Data[i];	
	for(i=0;i<4;i++)	
		DACAI_TXData[10+Data_Len+i]=DACAI_Over[i];
	DACAI_DataLen=Data_Len+14;
	DACAI_Send();
}
void DACAI_Curve_Clean(u16 Screen_ID,u16 Control_ID,u8 Channel)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0xB1;
	DACAI_TXData[2]=0x33;
	DACAI_TXData[3]=Screen_ID>>8;
	DACAI_TXData[4]=Screen_ID&0x00ff;
	DACAI_TXData[5]=Control_ID>>8;
	DACAI_TXData[6]=Control_ID&0x00ff;
	DACAI_TXData[7]=Channel;
	for(i=0;i<4;i++)	
		DACAI_TXData[8+i]=DACAI_Over[i];
	DACAI_DataLen=12;	
	DACAI_Send();
}
void DACAI_Text_UpData(u16 Screen_ID,u16 Control_ID,u8 Data_len,char *Text_Data)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0xB1;
	DACAI_TXData[2]=0x10;
	DACAI_TXData[3]=Screen_ID>>8;
	DACAI_TXData[4]=Screen_ID&0x00ff;
	DACAI_TXData[5]=Control_ID>>8;
	DACAI_TXData[6]=Control_ID&0x00ff;
	for(i=0;i<Data_len;i++)
		DACAI_TXData[7+i]=Text_Data[i];
	for(i=0;i<4;i++)	
		DACAI_TXData[10+Data_len+i]=DACAI_Over[i];
	DACAI_DataLen=Data_len+14;	
	DACAI_Send();
}
void DACAI_Text_Clean(u16 Screen_ID,u16 Control_ID)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0xB1;
	DACAI_TXData[2]=0x10;
	DACAI_TXData[3]=Screen_ID>>8;
	DACAI_TXData[4]=Screen_ID&0x00ff;
	DACAI_TXData[5]=Control_ID>>8;
	DACAI_TXData[6]=Control_ID&0x00ff;
	for(i=0;i<4;i++)	
		DACAI_TXData[7+i]=DACAI_Over[i];
	DACAI_DataLen=11;	
	DACAI_Send();
}

void DACAI_GoScreen(u16 Screen_ID)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0xB1;
	DACAI_TXData[2]=0x00;
	DACAI_TXData[3]=Screen_ID>>8;
	DACAI_TXData[4]=Screen_ID&0x00ff;
	for(i=0;i<4;i++)	
		DACAI_TXData[5+i]=DACAI_Over[i];
	DACAI_DataLen=9;	
	DACAI_Send();
}
void DACAI_TXRead_Text(u16 Screen_ID,u16 Control_ID)
{
	u16 i;
	DACAI_TXData[0]=DACAI_Head;
	DACAI_TXData[1]=0xB1;
	DACAI_TXData[2]=0x11;
	DACAI_TXData[3]=Screen_ID>>8;
	DACAI_TXData[4]=Screen_ID&0x00ff;
	DACAI_TXData[5]=Control_ID>>8;
	DACAI_TXData[6]=Control_ID&0x00ff;
	for(i=0;i<4;i++)	
		DACAI_TXData[7+i]=DACAI_Over[i];
	DACAI_DataLen=13;	
	DACAI_Send();
	while(!flag)
		DACAI_Read();
	Check_HMI();
}

