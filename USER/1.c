#include "main.h"

char Text_Data[7];
u8 Curve_Data[128];
s8 Y_Axe[8];
u8 In_Vol;
u16 New_Screen,New_Control;
u8 Daley_num;

u8 PGA_Level;									//PGA等级
u8 PGA;												//PGA放大倍数
s16 Zero_Skew;								//零点偏移量
float Zero_Skew_Vol;					//零点偏移电压值
float	Res_Rate;			  				//电阻采样比
float AD_Rate=20./65535.;				//ADS分辨率
float AD_Skew=1.000;					//ADS转换误差	
float Actual[128];						//AD检测电压缓存
							
u8 DaleyCycle=20;							//测量周期
u8 ScanNum;										//当前扫描次数
u8 Vol_Genre;									//电压类型
float AVG_Vol,RMS_Vol,Sprt_Vol,MMP_Vol;
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
u16 test_num;
s16 test;
float AD1,AD,PGA_Vol,AD_Vol,Vir_Vol;
/*								 子函数声明										*/
float Actual_Vol(void);
void KeyIf(void);
void TextIf(void);
void Dis_Vol(u16 Screen_ID,u16 Control_ID,u8 Data_len,float Text_Vol);
void Zero_Skew_Test(void);
void Show_Vol(void);
void Show_Curve(void);
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
	GPIO_SetBits(GPIOA,GPIO_Pin_11);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOA,GPIO_Pin_12);
}
void Main_EXTI_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line11;	
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
	
}

void init()
{
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_3);
	Main_GPIO_Init();
	Main_EXTI_Init();
	ADS_Init();
	
	DACAI_Init(115200);
	DACAI_HandShake();  
	DACAI_Rest();

}
/*									Main												*/
int main(void)
{

	NVIC_Configuration();
	init();
	delay_init();

	delay_ms(300);
	PGA10=0;
	PGA100=0;
	while(1)
	{
		Check_HMI();
		delay_ms(300);	
		KeyIf();
		TextIf();
		if(New_Screen==Detection_Screen)
		{
			if(Daley_num==DaleyCycle)
			{
				Daley_num=0;
				Show_Vol();
				if(Vol_Genre==AC)
					Show_Curve();
				AVG_Vol=RMS_Vol=Sprt_Vol=MMP_Vol=0;
				
			}
		}
	}
}

/*								 子函数												*/
float Actual_Vol(void)
{
	//float AD,PGA_Vol,AD_Vol,Vir_Vol;

	if(PGA_Level==0)
	{	PGA10=0;	PGA100=0;}
	if(PGA_Level==1)
	{	PGA10=1;	PGA100=0;}
	if(PGA_Level==2)
	{	PGA10=0;	PGA100=1;}

	Res_Rate=Res_Total/Res_Sample;																//运算电阻采样比
	PGA=100*PGA100+10*PGA10+!(PGA10||PGA10);											//计算放大倍数
	
	AD=AD_Run()-Zero_Skew;																				//零偏纠正
	AD_Vol=AD*AD_Rate*AD_Skew;																		//ADS输入电压
	PGA_Vol=AD_Vol/PGA;																				//PGA输入电压
	Vir_Vol=PGA_Vol*Res_Rate;																			//被测电压
	
	if(Vir_Vol<Min_Measure_Vol)
		In_Vol=No_In;
	if(Vir_Vol>Min_Measure_Vol)
		In_Vol=Yes_In;
	
//	if(AD<0x0800&&PGA_Level<2)
//		PGA_Level++;
//	if(AD>0xF000&&PGA_Level>0)
//		PGA_Level--;

	return Vir_Vol;
}
void KeyIf(void)
{
	if(KeyUp==1)
	{
		if(Operate_Screen==Cover_Screen)
		{
			if(Operate_Control==1&&Key_Buffer[0]==2&&Key_Buffer[1]==0x0D)
			{
				New_Screen=Detection_Screen;
				DACAI_GoScreen(Detection_Screen);
			}	
			if(Operate_Control==2&&Key_Buffer[0]==2&&Key_Buffer[1]==0x0D)
			{
				New_Screen=Config_Screen;
				DACAI_GoScreen(Config_Screen);
			}	
		}
		if(Operate_Screen==Detection_Screen)
		{
			if(Operate_Control==15&&Key_Buffer[0]==2&&Key_Buffer[1]==0x0D)
			{
				New_Screen=Cover_Screen;
				DACAI_GoScreen(Cover_Screen);
			}	
		}
		if(Operate_Screen==Config_Screen)
		{
			if(Operate_Control==4&&Key_Buffer[0]==2&&Key_Buffer[1]==0x0D)
				Zero_Skew_Test();
			if(Operate_Control==15&&Key_Buffer[0]==2&&Key_Buffer[1]==0x0D)
			{
				New_Screen=Cover_Screen;
				DACAI_GoScreen(Cover_Screen);
			}
		}
		KeyUp=0;
	}
}
void TextIf(void)
{
	u8 i;
	u8 ScanCycle_num;
	float AD_Skew_num;
	
	if(TextUp==1)
	{
		if(Operate_Screen==Config_Screen&&Operate_Control==AD_Skew_ID)
		{
			for(i=0;i<TextLen;i++)
				if(Text_Buffer[i]>='0')
					AD_Skew_num=AD_Skew_num*10+(Text_Buffer[i]-'0');
			if(AD_Skew_num>0&&AD_Skew_num<1200)
			{
				AD_Skew=AD_Skew_num/1000;
				DACAI_Bezz(50);
			}
			else DACAI_Bezz(200);
		}
		if(Operate_Screen==Config_Screen&&Operate_Control==DC_Cycle_ID&&Text_Buffer[0]>'0')
		{
			for(i=0;i<TextLen;i++)
				ScanCycle_num=ScanCycle_num*10+(Text_Buffer[i]-'0');
			DaleyCycle=ScanCycle_num;
		}
		TextUp=0;
	}
}
void Dis_Vol(u16 Screen_ID,u16 Control_ID,u8 Data_len,float Text_Vol)
{
	u8 i;
//	DACAI_Text_Clean(Screen_ID,Control_ID);
	sprintf(Text_Data,"%f",Text_Vol);
	Text_Data[Data_len-1]='V';
	DACAI_Text_UpData(Screen_ID,Control_ID,Data_len,Text_Data);
	for(i=0;i<8;i++)
	Text_Data[i]=0;
}
void Zero_Skew_Test(void)
{
	u8 i;
	PGA100=1;
	Zero_Skew=0;
	for(i=0;i<DaleyCycle;i++)
	{
		Zero_Skew=Zero_Skew+AD_Run();
		delay_us(500);
	}
	Zero_Skew=Zero_Skew/DaleyCycle;
	Zero_Skew_Vol=Zero_Skew*AD_Rate*AD_Skew;	
	
	sprintf(Text_Data,"%f",Zero_Skew_Vol); 
	DACAI_Text_UpData(Config_Screen,Zero_Skew_ID,5,Text_Data);
	DACAI_Bezz(10);
	PGA100=0;
}
void Show_Vol(void)
{
	u8 i;
	float AVG_Vol,RMS_Vol,Sprt_Vol;
	for(i=0;i<DaleyCycle;i++)
	{
		AVG_Vol=AVG_Vol+AVG[i];
		RMS_Vol=RMS_Vol+RMS[i];
		Sprt_Vol=Sprt_Vol+Sprt[i];
		MMP_Vol=MMP_Vol+MMP[i];
	}
	
	AVG_Vol=AVG_Vol/DaleyCycle;
	RMS_Vol=RMS_Vol/DaleyCycle;
	Sprt_Vol=Sprt_Vol/DaleyCycle;
	MMP_Vol=MMP_Vol/DaleyCycle;
	
	Dis_Vol(Detection_Screen,AVG_ID,6,AVG_Vol);
	Dis_Vol(Detection_Screen,MMP_ID,6,MMP_Vol);
	
	if(Vol_Genre==AC)
	{
		Dis_Vol(Detection_Screen,Sprt_ID,6,Sprt_Vol);
		Dis_Vol(Detection_Screen,RMS_ID,6,RMS_Vol);
	}
}

void Vol_Buf()
{
	u8 i;
	float AVG_Vol,RMS_Vol;
	for(i=0;i<ScanCycle;i++)
	{
		AVG_Vol=AVG_Vol+fabs(Actual[i]);
		RMS_Vol=Actual[i]*Actual[i]+RMS_Vol;
	}
	AVG[Daley_num]=AVG_Vol/ScanCycle;
	RMS[Daley_num]=sqrt(RMS_Vol/ScanCycle);
	MMP[Daley_num]=Max_Vol+fabs(Min_Vol);
	Sprt[Daley_num]=MMP[Daley_num]/2/1.4142;
	
	Max_Vol=Min_Vol=0;
	Daley_num++;
	ScanNum=0;
}
void Show_Curve(void)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		Y_Axe[i]=(MMP_Vol/8)*(4-i);
		Y_Axe[7-i]=0-Y_Axe[i];
	}
	for(i=0;i<8;i++)
		Dis_Vol(Detection_Screen,Y_Axe_ID+i,4,Y_Axe[i]);
	
	for(i=0;i<128;i++)
		Curve_Data[i]=(float)((Actual[i]+(MMP_Vol/2))/MMP_Vol)*0xff;
	DACAI_Curve_DisToEnd(Detection_Screen,Curve_ID,Channel_num,128,Curve_Data);
	MMP_Vol=0;
}
/*									中断  											*/
void EXTI15_10_IRQHandler()
{
	if(New_Screen==Detection_Screen&&ScanNum<ScanCycle&&Daley_num<DaleyCycle)
	{
		Actual[ScanNum]=Actual_Vol();
		if(Actual[ScanNum]>Max_Vol)
			Max_Vol=Actual[ScanNum];
		if(Actual[ScanNum]<Min_Vol)
			Min_Vol=Actual[ScanNum];
			ScanNum++;
	}
	if(ScanNum==ScanCycle)
		Vol_Buf();
	EXTI_ClearITPendingBit(EXTI_Line11);
}
