// KalmanFilter.h
#ifndef __KALMANFILTER_H
#define __KALMANFILTER_H

typedef struct 
	{
    float Q_angle;   // 过程噪声协方差（角度）
    float Q_bias;    // 过程噪声协方差（偏差）
    float R_measure; // 测量噪声协方差
    
    float angle;     // 计算后的角度
    float bias;      // 陀螺仪偏差
    float rate;      // 未经滤波的角速度
    
    float P[2][2];   // 误差协方差矩阵
} KalmanFilter;

void KalmanFilter_Init(KalmanFilter *kf, float Q_angle, float Q_bias, float R_measure);
float KalmanFilter_Update(KalmanFilter *kf, float newAngle, float newRate, float dt);

#endif
