// Attitude.h
#ifndef __ATTITUDE_H
#define __ATTITUDE_H

#include "KalmanFilter.h"

typedef struct {
    KalmanFilter roll_filter;
    KalmanFilter pitch_filter;
    float dt;          // 采样时间间隔（秒）
} Attitude;

void Attitude_Init(Attitude *att, float dt);
void Attitude_Update(Attitude *att, int16_t *accel, int16_t *gyro);
float Get_Roll(Attitude *att);
float Get_Pitch(Attitude *att);

#endif
