#include "ADS8517.H"

u16 AD_HDATA,AD_LDATA,AD_DATA;

void ADS_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStrcture;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_H8;
	GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOB,GPIO_Pin_H8);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	
	GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC,&GPIO_InitStrcture);
	GPIO_SetBits(GPIOC,GPIO_Pin_9);
}

void ADS_Init(void)
{
	ADS_GPIO_Init();
	CS=1;
	RC=1;
	BYTE=1;
}

s16 AD_Run(void)
{
	u16 x;
	RC=0;
	CS=0;
	while(BUSY);
	CS=1;
	RC=1;
	while(!BUSY)
	{
		x++;	//delay_us(1);
		if(x==300)
			return 0xff;
	}
	RC=1;
	BYTE=1;
	CS=0;
	AD_LDATA=GPIO_ReadInputData(GPIOB)>>8;
	CS=1;
	BYTE=0;
	CS=0;
	AD_HDATA=GPIO_ReadInputData(GPIOB)&0xFF00;
	CS=1;
	AD_DATA=AD_HDATA|AD_LDATA;
	return AD_DATA;
}
