#ifndef PWMCAPTURE_H
#define PWMCAPTURE_H

#pragma anon_unions

/**
 * @file M_File_pwmCapture.h
 * @author xfp23
 * @brief 此驱动用来捕获pwm信号
 * @note 在使用此驱动库前在CubeMX中使能对应捕获定时器的中断，分别使能捕获中断和全局中断
 *        上升沿为直接测量，下降沿为间接测量
 * @version 0.1
 * @date 2025-03-18
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "main.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define CAPTURE_TIM_BITS 32 // 定时器最大位宽

#define CONCAT(x) uint##x##_t
#define CAPTURE_TIM_BIT_T(x) CONCAT(x)

typedef CAPTURE_TIM_BIT_T(CAPTURE_TIM_BITS) capture_timbits_t;

typedef struct
{
    capture_timbits_t CCR1; // 寄存器CCR1的值
    capture_timbits_t CCR2; // 寄存器CCR2的值
} pwm_Capture_Int_t;

typedef struct
{
    TIM_HandleTypeDef *htim; // 定时器句柄
    uint32_t RiseChannel;    // 捕获上升沿通道
    uint32_t FallChannel;    // 捕获下降沿通道
} pwm_Capture_conf_t;

typedef struct
{
    HAL_TIM_ActiveChannel RiseChannel; // 上升沿通道
    HAL_TIM_ActiveChannel FallChannel; // 下降沿通道
} pwm_Capture_channelMap_t;               // 这个类型不是给你用的

typedef struct
{
    uint8_t isRiseEdge : 1;
    uint8_t isFallEdge : 1;
    uint8_t isCapComplete : 1; // 捕获完成
    uint8_t Reserve_bits : 5;  // 保留位
    bool capSwitch;            // 捕获开关,不可手动更改，由API自行管理
} pwm_Capture_Flag_t;

typedef struct
{
    uint32_t freq;    // PWM频率 单位: hz
    uint32_t pulseWidth; // 脉宽
    float duty;       // PWM占空比
    float period;     // pwm周期 单位: 秒

} pwm_Capture_Result_t; // pwm捕获结果

typedef enum
{
    PWM_CAPTURE_OK = 0x00,               // 操作成功
    PWM_CAPTURE_ERROR = 0xFF,            // 操作失败
    PWM_CAPTURE_INITIALIZED = 0x01,      // 已初始化
    PWM_CAPTURE_CHANNEL_MISMATCH = 0x02, // 通道不匹配
} PwmCaptureState_t;                     // 操作状态

typedef union
{
    struct
    {
        pwm_Capture_conf_t conf;          // 配置
        pwm_Capture_Int_t CCR;            // 寄存器值
        pwm_Capture_channelMap_t channelMap; // 这个字段不是给你用的
        pwm_Capture_Result_t result;      // 捕获结果
        pwm_Capture_Flag_t flag;          // 标志位
    };
} pwm_Capture_Class_t;

typedef pwm_Capture_Class_t *pwm_Capture_Handle_t; // 捕获句柄类型

PwmCaptureState_t pwmCapture_Init(pwm_Capture_Handle_t *handle, pwm_Capture_conf_t *conf);

void pwmCapture_Callback(pwm_Capture_Handle_t *pwm_Cap_handle, TIM_HandleTypeDef *htim);

PwmCaptureState_t pwmCapture_Delete(pwm_Capture_Handle_t *handle);

PwmCaptureState_t pwmCapture_Stop(pwm_Capture_Handle_t *handle);

PwmCaptureState_t pwmCapture_Start(pwm_Capture_Handle_t *handle);

PwmCaptureState_t pwmCapture_Reset(pwm_Capture_Handle_t *handle);

PwmCaptureState_t pwmCapture_Delete(pwm_Capture_Handle_t *handle);

uint32_t pwmCapture_getPulseWidth(pwm_Capture_Handle_t handle);

uint32_t pwmCapture_getFreq(pwm_Capture_Handle_t handle);

float pwmCapture_getDuty(pwm_Capture_Handle_t handle);

uint32_t pwmCapture_getPeriod(pwm_Capture_Handle_t handle);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !M_FILE_PWMCAPTURE_H
