#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {

    /* 控制器增益 */
    float Kp;  // 比例增益
    float Ki;  // 积分增益
    float Kd;  // 微分增益

    /* 微分低通滤波器时间常数 */
    float tau; // 用于平滑微分项的滤波器常数

    /* 输出限制 */
    float limMin;  // 输出的最小限制
    float limMax;  // 输出的最大限制
    
    /* 积分限制 */
    float limMinInt;  // 积分项的最小限制
    float limMaxInt;  // 积分项的最大限制

    /* 采样时间（单位：秒） */
    float T;  // 控制器的时间步长或采样时间

    /* 控制器的“记忆” */
    float integrator;  // 积分项的当前值
    float prevError;   // 上一个误差，用于计算积分项
    float differentiator;  // 微分项的当前值
    float prevMeasurement; // 上一个测量值，用于计算微分项

    /* 控制器输出 */
    float out;  // 控制器的最终输出

} PIDController_Class_t;

typedef struct{
	float kp;
	float ki;
	float kd;
	float limMin; 	  // 输出的最小限制
    float limMax; 	  // 输出的最大限制
	float tau;		  // 用于平滑微分项的滤波器常数
	float T; 		  // 控制器的时间步长或采样时间
	float limMinInt;  // 积分项的最小限制
    float limMaxInt;  // 积分项的最大限制
} PIDController_Conf_t;

typedef PIDController_Class_t *PIDController_Handle_t; // pid句柄

void  PIDController_Init(PIDController_Handle_t *handle,PIDController_Conf_t *conf);
float PIDController_Update(PIDController_Handle_t *handle, float setpoint, float measurement);

#ifdef __cpluscplus
}
#endif
#endif
