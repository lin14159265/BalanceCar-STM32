#include "stm32f10x.h"                  // Device header
#include "Attitude.h"
#include <math.h>

#define RAD_TO_DEG 57.295779513f

void Attitude_Init(Attitude *att, float dt) {
    att->dt = dt;
    // 初始化卡尔曼滤波器参数（可根据实际传感器特性调整）
    KalmanFilter_Init(&att->roll_filter, 0.001f, 0.003f, 0.5f);
    KalmanFilter_Init(&att->pitch_filter, 0.001f, 0.003f, 0.5f);
}

void Attitude_Update(Attitude *att, int16_t *accel, int16_t *gyro) {
    // 加速度计原始数据转换为g值（根据传感器量程±16g配置）
    float ax = accel[0] / 2048.0f;
    float ay = accel[1] / 2048.0f;
    float az = accel[2] / 2048.0f;

    // 计算加速度计姿态角（单位：弧度）
    float accel_roll = atan2(ay, sqrt(ax*ax + az*az));
    float accel_pitch = atan2(-ax, sqrt(ay*ay + az*az));

    // 陀螺仪原始数据转换为角速度（根据±2000dps配置）
    float gyro_roll = gyro[0] / 16.384f;  // 转换为°/s
    float gyro_pitch = gyro[1] / 16.384f;

    // 应用卡尔曼滤波
    att->roll_filter.angle = KalmanFilter_Update(&att->roll_filter, 
        accel_roll * RAD_TO_DEG, gyro_roll, att->dt);
    att->pitch_filter.angle = KalmanFilter_Update(&att->pitch_filter, 
        accel_pitch * RAD_TO_DEG, gyro_pitch, att->dt);
}

float Get_Roll(Attitude *att) {
    return att->roll_filter.angle;
}

float Get_Pitch(Attitude *att) {
    return att->pitch_filter.angle;
}
