#ifndef  _CONTROL_H
#define  _CONTROL_H

#include "sys.h" 

//#define CLAMP_SPEED(val, max) \
//    do { \
//        typeof(val) _val = (val); \
//        typeof(max) _max = (max); \
//        if (_val > _max) _val = _max; \
//        else if (_val < -_max) _val = -_max; \
//        (val) = _val; \
//    } while(0)


int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int Target,int encoder_left,int encoder_right);
int Turn(int gyro_Z,int RC);
void EXTI9_5_IRQHandler(void);
#endif

