#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void SETPWM(int32_t MOTOA,int32_t MOTOB);
void PWM_Xianfu(int32_t max,int32_t *MOTOA,int32_t *MOTOB);//�޷�
void PWM_XIANFU(int32_t max,int32_t *MOTOA,int32_t *MOTOB);//��������޷�
int32_t read_encoder2(void);
int32_t read_encoder3(void);
	
#endif
