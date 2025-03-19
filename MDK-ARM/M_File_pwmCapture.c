#include "M_File_pwmCapture.h"
#include "tim.h"
#include "stdlib.h"
#include "string.h"

#ifndef ON
#define ON (1)
#endif

#ifndef OFF
#define OFF (0)
#endif

/**
 * @brief 初始化pwm输入捕获
 *
 * @param handle 输入捕获句柄
 * @param conf 配置 pwm_capture_conf_t 结构体写入配置
 */
PwmCaptureState_t pwmCapture_Init(pwm_Capture_Handle_t *handle, pwm_Capture_conf_t *conf)
{
    if (handle == NULL || *handle != NULL)
    {
        return PWM_CAPTURE_INITIALIZED;
    }

    *handle = calloc(1, sizeof(pwm_Capture_Class_t));
    if (*handle == NULL)
    {
        return PWM_CAPTURE_ERROR;
    }

    (*handle)->conf.FallChannel = conf->FallChannel;
    (*handle)->conf.RiseChannel = conf->RiseChannel;
    (*handle)->conf.htim = conf->htim;

    // 处理通道映射
    switch ((*handle)->conf.FallChannel)
    {
        case TIM_CHANNEL_1: (*handle)->channelMap.FallChannel = HAL_TIM_ACTIVE_CHANNEL_1; break;
        case TIM_CHANNEL_2: (*handle)->channelMap.FallChannel = HAL_TIM_ACTIVE_CHANNEL_2; break;
        case TIM_CHANNEL_3: (*handle)->channelMap.FallChannel = HAL_TIM_ACTIVE_CHANNEL_3; break;
        case TIM_CHANNEL_4: (*handle)->channelMap.FallChannel = HAL_TIM_ACTIVE_CHANNEL_4; break;
        case TIM_CHANNEL_ALL: (*handle)->channelMap.FallChannel = HAL_TIM_ACTIVE_CHANNEL_CLEARED; break;
        default: return PWM_CAPTURE_CHANNEL_MISMATCH;
    }

    switch ((*handle)->conf.RiseChannel)
    {
        case TIM_CHANNEL_1: (*handle)->channelMap.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_1; break;
        case TIM_CHANNEL_2: (*handle)->channelMap.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_2; break;
        case TIM_CHANNEL_3: (*handle)->channelMap.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_3; break;
        case TIM_CHANNEL_4: (*handle)->channelMap.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_4; break;
        case TIM_CHANNEL_ALL: (*handle)->channelMap.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_CLEARED; break;
        default: return PWM_CAPTURE_CHANNEL_MISMATCH;
    }

    __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC2);
    HAL_TIM_IC_Start_IT((*handle)->conf.htim, (*handle)->conf.RiseChannel);
    HAL_TIM_IC_Start_IT((*handle)->conf.htim, (*handle)->conf.FallChannel);
    (*handle)->flag.capSwitch = true;
    return PWM_CAPTURE_OK;
}

/**
 * @brief 中断回调
 * @note 1. 回调函数 在hal库中的  HAL_TIM_IC_CaptureCallback() 函数里调用
 *       2. if(htim->Instance == TIMx) 调用前判断定时器触发
 * @param pwm_Cap_handle
 * @param htim 传入HAL_TIM_IC_CaptureCallback()函数的形参就可以
 */
void pwmCapture_Callback(pwm_Capture_Handle_t *handle, TIM_HandleTypeDef *htim)
{
    if (handle == NULL || *handle == NULL) return;
    if (!(*handle)->flag.capSwitch) return;

    if (htim->Channel == (*handle)->channelMap.RiseChannel)
    {
        if ((*handle)->flag.isRiseEdge == OFF)
        {
            (*handle)->flag.isRiseEdge = ON;
            __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC1);
        }
        if ((*handle)->flag.isRiseEdge == ON)
        {
            __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC1);
            (*handle)->CCR.CCR1 = __HAL_TIM_GET_COMPARE((*handle)->conf.htim, (*handle)->conf.RiseChannel);
        }
    }

    if (htim->Channel == (*handle)->channelMap.FallChannel)
    {
        __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC2);
        (*handle)->CCR.CCR2 = __HAL_TIM_GET_COMPARE((*handle)->conf.htim, (*handle)->conf.FallChannel);
        (*handle)->flag.isFallEdge = ON;
    }

    if ((*handle)->flag.isFallEdge == ON && (*handle)->flag.isRiseEdge == ON)
    {
        memset(&(*handle)->flag, OFF,1);

        /** 开始计算 */
        // 周期
        (*handle)->result.period = (float)(*handle)->CCR.CCR1 * 1E-6;

        // 占空比
        (*handle)->result.duty = ((float)(*handle)->CCR.CCR2 / (float)(*handle)->CCR.CCR1) * 100;
        (*handle)->result.pulseWidth = (*handle)->CCR.CCR2;

        // 频率
        (*handle)->result.freq = 1.00 / ((*handle)->result.period);

        (*handle)->flag.isCapComplete = ON;
        memset(&(*handle)->CCR, 0, sizeof(pwm_Capture_Int_t));
    }
}

/**
 * @brief 停止PWM捕获输入
 *
 * @param handle
 */
PwmCaptureState_t pwmCapture_Stop(pwm_Capture_Handle_t *handle)
{
    if (handle == NULL || *handle == NULL) return PWM_CAPTURE_ERROR;
    __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC2);
    (*handle)->flag.isCapComplete = OFF;
    (*handle)->flag.capSwitch = false;
    HAL_TIM_IC_Stop_IT((*handle)->conf.htim, (*handle)->conf.RiseChannel);
    HAL_TIM_IC_Stop_IT((*handle)->conf.htim, (*handle)->conf.FallChannel);
    return PWM_CAPTURE_OK;
}

/**
 * @brief 开启pwm捕获
 * 
 * @param handle
 */
PwmCaptureState_t pwmCapture_Start(pwm_Capture_Handle_t *handle)
{
    if (handle == NULL || *handle == NULL) return PWM_CAPTURE_ERROR;
    __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC2);
    HAL_TIM_IC_Start_IT((*handle)->conf.htim, (*handle)->conf.RiseChannel);
    HAL_TIM_IC_Start_IT((*handle)->conf.htim, (*handle)->conf.FallChannel);
    (*handle)->flag.capSwitch = true;
    return PWM_CAPTURE_OK;
}

/**
 * @brief 重启复位pwm捕获输入
 *
 * @param handle
 */
PwmCaptureState_t pwmCapture_Reset(pwm_Capture_Handle_t *handle)
{ 
    if (handle == NULL || *handle == NULL) return PWM_CAPTURE_ERROR;
    HAL_TIM_IC_Stop_IT((*handle)->conf.htim, (*handle)->conf.RiseChannel);
    HAL_TIM_IC_Stop_IT((*handle)->conf.htim, (*handle)->conf.FallChannel);
    // 清空
    memset(&(*handle)->result, 0, sizeof(pwm_Capture_Result_t));
    memset(&(*handle)->flag, 0, sizeof(pwm_Capture_Flag_t));
    memset(&(*handle)->CCR, 0, sizeof(pwm_Capture_Int_t));
    (*handle)->flag.capSwitch = true;
    __HAL_TIM_CLEAR_FLAG((*handle)->conf.htim, TIM_FLAG_CC1);
    HAL_TIM_IC_Start_IT((*handle)->conf.htim, (*handle)->conf.RiseChannel);
    HAL_TIM_IC_Start_IT((*handle)->conf.htim, (*handle)->conf.FallChannel);
    return PWM_CAPTURE_OK;
}

/**
 * @brief 删除pwm捕获输入
 *
 * @param handle
 */
PwmCaptureState_t pwmCapture_Delete(pwm_Capture_Handle_t *handle)
{
    if (handle == NULL || *handle == NULL) return PWM_CAPTURE_ERROR;
    HAL_TIM_IC_Stop_IT((*handle)->conf.htim, (*handle)->conf.RiseChannel);
    HAL_TIM_IC_Stop_IT((*handle)->conf.htim, (*handle)->conf.FallChannel);
    free(*handle);
    *handle = NULL;
    return PWM_CAPTURE_OK;
}

/**
 * @brief 获取捕获结果的频率 单位: hz
 * 
 * @param handle 
 * @return uint32_t 
 */
uint32_t pwmCapture_getFreq(pwm_Capture_Handle_t handle)
{
    if(handle == NULL) return 0;
     return handle->result.freq;
}

/**
 * @brief 获取捕获结果的脉宽
 * 
 * @param handle 
 * @return uint32_t 
 */
uint32_t pwmCapture_getPulseWidth(pwm_Capture_Handle_t handle)
{
    if(handle == NULL) return 0;
    return handle->result.pulseWidth;
}

/**
 * @brief 获取捕获结果的占空比
 * 
 * @param handle 
 * @return float 
 */
float pwmCapture_getDuty(pwm_Capture_Handle_t handle)
{
    if(handle == NULL) return 0;
    return handle->result.duty;
}

/**
 * @brief 获取捕获结果的周期 单位 : 秒
 * 
 * @param handle 
 * @return uint32_t 
 */
uint32_t pwmCapture_getPeriod(pwm_Capture_Handle_t handle)
{
    if(handle == NULL) return 0;
    return handle->result.period;
}