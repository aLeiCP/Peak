#ifndef __ADS8517_H
#define __ADS8517_H

#include "stm32f10x_gpio.h"
#include "delay.h"

#define BUSY PCin(6)
#define CS   PCout(7)
#define RC   PCout(8)
#define BYTE PCout(9)

void ADS_GPIO_Init(void);
void ADS_Init(void);
s16 AD_Run(void);

#endif
