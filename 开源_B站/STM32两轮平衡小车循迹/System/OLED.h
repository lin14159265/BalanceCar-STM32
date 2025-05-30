#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"

void OLED_Init(void);//OLED��ʼ��
void OLED_Clear(void);//OLED����
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);//OLED��ʾһ���ַ�
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);//OLED��ʾ�ַ���
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);//OLED��ʾ���֣�ʮ���ƣ�������
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);//OLED��ʾ���֣�ʮ���ƣ�����������
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);//OLED��ʾ���֣�ʮ�����ƣ�������
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);//OLED��ʾ���֣������ƣ�������

#endif
