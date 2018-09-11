#include "main.h"

char Text_Data[7];
u8 Curve_Data[256];
float Y_Axe[8];
u8 In_Vol;
u8 Daley_num;
u8 tad[3]={0x7f,0xb0,0x30};

u8 PGA;												//PGA放大倍数
u8 PGA_Level;									//PGA等级
s16 Zero_Offset[3];								//零点偏移量
s16 Max_AD,Min_AD=0x0700;
float Zero_Offset_Vol;					//零点偏移电压值
float	Res_Rate;			  				//电阻采样比
float AD_Rate=20./65535.;				//ADS分辨率
float Full_Offset=1.000;					//ADS转换误差	
float Full_Offset_Vol;
float InVol_Ratio=1;
float Actual[128];						//AD检测电压缓存
							
u8 DaleyCycle=20;							//测量周期
u8 ScanNum;										//当前扫描次数
u8 Vol_Genre;									//电压类型
float AVG_Vol,RMS_Vol,Sprt_Vol,MMP_Vol;//平均值,均方根,有效值,峰峰值
float Max_Vol;								//最大值
float Min_Vol;								//最小值
float MMP_Vol;
float AVG[100];
float RMS[100];
float Sprt[100];
float MMP[100];
float Max[100];
float Min[100];
u8 d;
u8 Detection;
u8 In_YesNo,Clean_Flag;
u8 ScanCycle_num;
	float AD,PGA_Vol,AD_Vol,Vir_Vol;
/*								 子函数声明										*/
float Actual_Vol(void);
void Vol_Buf(void);
void Dis_Vol(u16 Screen_ID,u16 Control_ID,u8 Data_len,float Text_Vol);
void Zero_Offset_Test(void);
void Full_Offset_Test(void);
void Show_Vol(void);
void Show_Curve(void);
void KeyIf(void);
void TextIf(void);
void Dis_Screen(void);
void Clean_Dis(void);
/*									Init												*/
void Main_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStrcture;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOC,GPIO_Pin_10);
		
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOC,GPIO_Pin_11);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrcture);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrcture);
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}
void Main_EXTI_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line11;	
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI_No()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;								
	NVIC_Init(&NVIC_InitStructure);
}
void EXTI_Yes()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}
void init()
{
	RCC_HSEConfig(RCC_HSE_ON);
	RCC_PLLConfig(RCC_PLLSource_HSE_Div2,RCC_PLLMul_6);	
	delay_init();
	DACAI_Init(115200);
	Main_GPIO_Init();
	Main_EXTI_Init();
	ADS_Init();
	PGA10=0;
	PGA100=0;
	DACAI_HandShake();  
	DACAI_Rest();

}
/*									Main												*/
int main(void)
{
	init();
	NVIC_Configuration();
	delay_ms(500);
	while(1)
	{	
		DACAI_ReadScreen();
		Check_HMI();
		KeyIf();
		TextIf();
		Dis_Screen();
		if(New_Screen==Cover_Screen||New_Screen==Config_Screen)
		{
			Detection=0;
			Daley_num=ScanNum=MMP_Vol=AVG_Vol=RMS_Vol=Sprt_Vol=0;
			EXTI_No();		In_YesNo=In_No;		Clean_Dis();
		}
		else
		{
			if(Detection==0)	EXTI_Yes();
			Detection=1;
			In_YesNo=In_Yes;
		}
		if(In_YesNo==In_No)
		{
			ScanNum=Daley_num=0;
			AVG_Vol=RMS_Vol=Sprt_Vol=MMP_Vol=0;
			Clean_Dis();
		}
			delay_ms(200);
	}
}

/*								 子函数												*/
float Actual_Vol(void)
{
	if(PGA_Level==0)
	{	PGA10=0;	PGA100=0;}
	if(PGA_Level==1)
	{	PGA10=1;	PGA100=0;}
	if(PGA_Level==2)
	{	PGA10=0;	PGA100=1;}

	Res_Rate=Res_Total/Res_Sample;																//运算电阻采样比
	PGA=100*PGA100+10*PGA10+!(PGA100||PGA10);											//计算放大倍数
	
	AD=AD_Run()-Zero_Offset[PGA_Level];														//零偏纠正
	AD_Vol=AD*AD_Rate*Full_Offset;																//ADS输入电压
	PGA_Vol=AD_Vol/PGA;																						//PGA输入电压
	Vir_Vol=PGA_Vol*Res_Rate;																			//被测电压

	if(fabs(AD_Vol)>9.5&&PGA_Level>0)
		PGA_Level--;
	return Vir_Vol;
}
void Vol_Buf(void)
{
	u8 i;
	AVG_Vol=RMS_Vol=Sprt_Vol=MMP_Vol=0;
	for(i=0;i<ScanCycle;i++)
	{
		AVG_Vol=AVG_Vol+fabs(Actual[i]);
		RMS_Vol=Actual[i]*Actual[i]+RMS_Vol;
	}
	AVG[Daley_num]=AVG_Vol/ScanCycle;
	RMS[Daley_num]=sqrt(RMS_Vol/ScanCycle);
	MMP[Daley_num]=Max_Vol+fabs(Min_Vol);
	Sprt[Daley_num]=MMP[Daley_num]/2/1.4142;
	if(MMP[Daley_num]<0.5&&PGA_Level==2)
		{In_YesNo=In_No;	PGA_Level=0;}
	else if(MMP[Daley_num]<0.5&&PGA_Level<2)
		PGA_Level++;
	Daley_num++;
	ScanNum=0;
}
void Dis_Vol(u16 Screen_ID,u16 Control_ID,u8 Data_len,float Text_Vol)
{
	u8 i;
	sprintf(Text_Data,"%f",Text_Vol);
	Text_Data[Data_len-1]='V';
	DACAI_Text_UpData(Screen_ID,Control_ID,Data_len,Text_Data);
	for(i=0;i<8;i++)
	Text_Data[i]=0;
}
void Zero_Offset_Test(void)
{
	u16 i;
	for(i=0;i<DaleyCycle*5;i++)
	{
		PGA100=0;	PGA10=0;
		Zero_Offset[0]=Zero_Offset[0]+AD_Run();
		PGA100=0;	PGA10=1;
		Zero_Offset[1]=Zero_Offset[1]+AD_Run();
		PGA100=1;	PGA10=0;
		Zero_Offset[2]=Zero_Offset[2]+AD_Run();
		delay_us(250);
	}
	Zero_Offset[0]=Zero_Offset[0]/DaleyCycle*5;
	Zero_Offset[1]=Zero_Offset[1]/DaleyCycle*5;
	Zero_Offset[2]=Zero_Offset[2]/DaleyCycle*5;
	Zero_Offset_Vol=Zero_Offset[2]*AD_Rate/100*Res_Rate;	
	sprintf(Text_Data,"%f",Zero_Offset_Vol); 
	DACAI_Text_UpData(Config_Screen,Zero_Offset_ID,5,Text_Data);
	DACAI_Bezz(10);
	PGA100=0;PGA10=0;
}
void Full_Offset_Test(void)
{
	u16 i;
	PGA100=0;
	PGA10=0;
	Res_Rate=Res_Total/Res_Sample;
	for(i=0;i<DaleyCycle*5;i++)
	{
		Full_Offset=Full_Offset+AD_Run()-Zero_Offset[0];
		Full_Offset_Vol=Full_Offset_Vol+(Full_Offset*AD_Rate*Res_Rate);
		delay_us(250);
	}
	Full_Offset_Vol=Full_Offset_Vol/DaleyCycle*5;
	Full_Offset=100./Full_Offset_Vol;
	
	sprintf(Text_Data,"%f",Full_Offset); 
	DACAI_Text_UpData(Config_Screen,Full_Offset_ID,5,Text_Data);
	DACAI_Bezz(10);

}
void Show_Vol(void)
{
	u8 i;
	AVG_Vol=RMS_Vol=Sprt_Vol=MMP_Vol=0;
	for(i=0;i<Daley_num;i++)
	{
		AVG_Vol=AVG_Vol+AVG[i];
		RMS_Vol=RMS_Vol+RMS[i];
		Sprt_Vol=Sprt_Vol+Sprt[i];
		MMP_Vol=MMP_Vol+MMP[i];
	}

	AVG_Vol=AVG_Vol/Daley_num*InVol_Ratio;
	RMS_Vol=RMS_Vol/Daley_num*InVol_Ratio;
	Sprt_Vol=Sprt_Vol/Daley_num*InVol_Ratio;
	MMP_Vol=MMP_Vol/Daley_num*InVol_Ratio;

	if(In_YesNo==In_Yes)
	{
		Dis_Vol(Detection_Screen,AVG_ID,6,AVG_Vol);
		Dis_Vol(Detection_Screen,MMP_ID,6,MMP_Vol);
		if(Vol_Genre==AC)
		{
			Dis_Vol(Detection_Screen,Sprt_ID,6,Sprt_Vol);
			Dis_Vol(Detection_Screen,RMS_ID,6,RMS_Vol);
		}
	}
}
void Show_Curve(void)
{
	u8 i;
	if(MMP_Vol<10)
		MMP_Vol=10;
	MMP_Vol=MMP_Vol*1.2/InVol_Ratio;
	for(i=0;i<4;i++)
	{
		Y_Axe[i]=(Max_Vol*1.2/4)*(4-i)*InVol_Ratio;
		Y_Axe[7-i]=(Min_Vol*1.2/4)*(4-i)*InVol_Ratio;
	}
	Max_Vol=Min_Vol=0;
	for(i=0;i<8;i++)
		Dis_Vol(Detection_Screen,Y_Axe_ID+i,6,Y_Axe[i]);
	for(i=0;i<128;i++)
		Curve_Data[128+i]=Curve_Data[i]=((float)((Actual[i]+(MMP_Vol/2))/MMP_Vol))*0xff;
	DACAI_Curve_DisToEnd(Detection_Screen,Curve_ID,Channel_num,256,Curve_Data);	
	MMP_Vol=0;
}

void KeyIf(void)
{
	if(KeyUp==1)
	{
		if((Operate_Screen==Config_Screen)&&(Operate_Control==Zero_Key_ID))
		{
			if(Key_Buffer[0]==0x02&&Key_Buffer[1]==0x0D)
				Zero_Offset_Test();
		}
		Operate_Screen=Operate_Control=0;
		KeyUp=0;
	}
}
void TextIf(void)
{
	u8 i;
	u8 Text_num=0;
	if(TextUp==1)
	{
		if(Operate_Screen==Config_Screen&&Operate_Control==DC_Cycle_ID)
		{
			if(Text_Buffer[0]>'0')
			{
				for(i=0;i<TextLen;i++)
					Text_num=Text_num*10+(Text_Buffer[i]-'0');
				DaleyCycle=Text_num;
			}
		}
		if(Operate_Screen==Config_Screen&&Operate_Control==Ratio_ID)
		{
			if(Text_Buffer[0]>'0')
			{
				for(i=0;i<TextLen;i++)
					Text_num=Text_num*10+(Text_Buffer[i]-'0');
				InVol_Ratio=Text_num;
			}
		}
		TextUp=0;
	}
}
void Dis_Screen(void)
{
	u8 i;
	if(Daley_num>=DaleyCycle)
	{
		EXTI_No();
		switch(New_Screen)
		{
			case Detection_Screen:
				Show_Vol();
				if(Vol_Genre==AC)
					Show_Curve();
				break;
		{		
			case AVG_Screen:
				AVG_Vol=0;
				for(i=0;i<Daley_num;i++)
					AVG_Vol=AVG_Vol+AVG[i];
				AVG_Vol=AVG_Vol/Daley_num*InVol_Ratio;
				Dis_Vol(AVG_Screen,AVG_ID,6,AVG_Vol);
				break;
			
			case RMS_Screen:
				RMS_Vol=0;
				for(i=0;i<Daley_num;i++)
					RMS_Vol=RMS_Vol+RMS[i];
				RMS_Vol=RMS_Vol/Daley_num*InVol_Ratio;
				Dis_Vol(RMS_Screen,RMS_ID,6,RMS_Vol);
				break;
			
			case Sprt_Screen:
				for(i=0;i<Daley_num;i++)
					Sprt_Vol=Sprt_Vol+Sprt[i];
				Sprt_Vol=Sprt_Vol/Daley_num*InVol_Ratio;
				Dis_Vol(Sprt_Screen,Sprt_ID,6,Sprt_Vol);
				break;
			
			case MMP_Screen:
				for(i=0;i<Daley_num;i++)
					MMP_Vol=MMP_Vol+MMP[i];
				MMP_Vol=MMP_Vol/Daley_num*InVol_Ratio;
				Dis_Vol(MMP_Screen,MMP_ID,6,MMP_Vol);
				break;}

		}		
		Daley_num=0;
		EXTI_Yes();
	}
}

void Clean_Dis(void)
{
	u8 i;
	for(i=0;i<5;i++)
	{
		Dis_Vol(Detection_Screen,AVG_ID+i,6,0);
		Dis_Vol(AVG_Screen+i,AVG_ID+i,6,0);
	}
	for(i=0;i<8;i++)
		Dis_Vol(Detection_Screen,Y_Axe_ID+i,6,0);
	DACAI_Curve_Clean(Detection_Screen,Curve_ID,Channel_num);
 }
/*									中断  											*/
void EXTI15_10_IRQHandler()
{
	if(Detection==1)
	{
		if((ScanNum<ScanCycle)&&(Daley_num<DaleyCycle))
		{
			Actual[ScanNum]=Actual_Vol();
			if(In_YesNo==In_No&&Actual[ScanNum]>0.5)
				In_YesNo=In_Yes;
			if(In_YesNo==In_Yes)
			{
				if(Actual[ScanNum]>Max_Vol)
					Max_Vol=Actual[ScanNum];
				if(Actual[ScanNum]<Min_Vol)
					Min_Vol=Actual[ScanNum];
				if((ScanNum==0&&((Actual[0])>=0.0)&&((Actual[0])<0.5))||(ScanNum>0))
					ScanNum++;
			}
		}
		if(ScanNum==ScanCycle)
			Vol_Buf();
	}
	EXTI_ClearITPendingBit(EXTI_Line11);
}
