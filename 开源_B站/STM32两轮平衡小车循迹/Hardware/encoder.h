#include "stm32f10x.h"                  // Device header

#ifndef __ENCODER_H
#define __ENCODER_H

void Encoder_Z_Init(void);
int16_t Encoder_Z_Get(void);
void Encoder_Y_Init(void);
int16_t Encoder_Y_Get(void);

#endif
