
#ifndef __PID_H
#define __PID_H

uint8_t Stop_Off(float angle);
int32_t balance(float Angle,float zhongzhi,float Gyro);
int32_t velocity(float Pitch,int32_t encoder_left,int32_t encoder_right);
int32_t turn(int32_t encoder_left,int32_t encoder_right,float gyro);
void HC_SR04_gensui(int16_t HC_SRO4);
#endif
