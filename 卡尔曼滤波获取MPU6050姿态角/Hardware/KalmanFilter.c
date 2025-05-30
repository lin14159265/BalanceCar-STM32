#include "stm32f10x.h"                  // Device header
#include "KalmanFilter.h"
#include <math.h>

void KalmanFilter_Init(KalmanFilter *kf, float Q_angle, float Q_bias, float R_measure)
	{
		kf->Q_angle = Q_angle;
		kf->Q_bias = Q_bias;
		kf->R_measure = R_measure;

		kf->angle = 0.0f;
		kf->bias = 0.0f;

		// 初始化误差协方差矩阵
		kf->P[0][0] = 0.0f;
		kf->P[0][1] = 0.0f;
		kf->P[1][0] = 0.0f;
		kf->P[1][1] = 0.0f;
}

float KalmanFilter_Update(KalmanFilter *kf, float newAngle, float newRate, float dt)
	{
		// 预测阶段
		kf->rate = newRate - kf->bias;
		kf->angle += dt * kf->rate;

		// 更新误差协方差矩阵
		kf->P[0][0] += dt * (dt*kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
		kf->P[0][1] -= dt * kf->P[1][1];
		kf->P[1][0] -= dt * kf->P[1][1];
		kf->P[1][1] += kf->Q_bias * dt;

		// 计算卡尔曼增益
		float S = kf->P[0][0] + kf->R_measure;
		float K[2];
		K[0] = kf->P[0][0] / S;
		K[1] = kf->P[1][0] / S;

		// 更新估计值
		float y = newAngle - kf->angle;
		kf->angle += K[0] * y;
		kf->bias += K[1] * y;

		// 更新协方差矩阵
		float P00_temp = kf->P[0][0];
		float P01_temp = kf->P[0][1];

		kf->P[0][0] -= K[0] * P00_temp;
		kf->P[0][1] -= K[0] * P01_temp;
		kf->P[1][0] -= K[1] * P00_temp;
		kf->P[1][1] -= K[1] * P01_temp;

		return kf->angle;
}
