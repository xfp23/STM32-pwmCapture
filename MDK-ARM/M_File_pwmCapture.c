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
void pwmCapture_Init(pwm_Capture_Handle_t *handle, pwm_Capture_conf_t *conf)
{

    memset((void *)handle, 0, sizeof(pwm_Capture_Handle_t));
    // 匹配通道
    handle->conf.FallChannel = conf->FallChannel;
    handle->conf.RiseChannel = conf->RiseChannel;
    handle->conf.htim = conf->htim;
    if (handle->conf.FallChannel == TIM_CHANNEL_1)
    {
        handle->callBack_param.FallChannel = HAL_TIM_ACTIVE_CHANNEL_1;
    }
    else if (handle->conf.FallChannel == TIM_CHANNEL_2)
    {
        handle->callBack_param.FallChannel = HAL_TIM_ACTIVE_CHANNEL_2;
    }
    else if (handle->conf.FallChannel == TIM_CHANNEL_3)
    {
        handle->callBack_param.FallChannel = HAL_TIM_ACTIVE_CHANNEL_3;
    }
    else if (handle->conf.FallChannel == TIM_CHANNEL_4)
    {
        handle->callBack_param.FallChannel = HAL_TIM_ACTIVE_CHANNEL_4;
    }
    else if (handle->conf.FallChannel == TIM_CHANNEL_ALL)
    {
        handle->callBack_param.FallChannel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }

    if (handle->conf.RiseChannel == TIM_CHANNEL_1)
    {
        handle->callBack_param.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_1;
    }
    else if (handle->conf.RiseChannel == TIM_CHANNEL_2)
    {
        handle->callBack_param.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_2;
    }
    else if (handle->conf.RiseChannel == TIM_CHANNEL_3)
    {
        handle->callBack_param.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_3;
    }
    else if (handle->conf.RiseChannel == TIM_CHANNEL_4)
    {
        handle->callBack_param.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_4;
    }
    else if (handle->conf.RiseChannel == TIM_CHANNEL_ALL)
    {
        handle->callBack_param.RiseChannel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }

   __HAL_TIM_CLEAR_FLAG(handle->conf.htim, TIM_FLAG_CC1);
    HAL_TIM_IC_Start_IT(handle->conf.htim, handle->conf.RiseChannel);
    HAL_TIM_IC_Start_IT(handle->conf.htim, handle->conf.FallChannel);
}

/**
 * @brief 中断回调
 * @note 1. 回调函数 在hal库中的  HAL_TIM_IC_CaptureCallback() 函数里调用
 *       2. if(htim->Instance == TIM7) 调用前判断定时器触发
 * @param pwm_Cap_handle
 * @param htim 传入HAL_TIM_IC_CaptureCallback()函数的形参就可以
 */
void pwmCapture_Callback(pwm_Capture_Handle_t *handle, TIM_HandleTypeDef *htim)
{
    if (htim->Channel == handle->callBack_param.RiseChannel)
    {
        if (handle->flag.isFirstRiseEdge == OFF && handle->flag.isSecondRiseEdge == OFF)
        {
            handle->flag.isFirstRiseEdge = ON;
            handle->CCR.firstEdgeRise_CCR = __HAL_TIM_GET_COMPARE(handle->conf.htim, handle->conf.RiseChannel);
			 __HAL_TIM_CLEAR_FLAG(handle->conf.htim, TIM_FLAG_CC1);
        }
        else if (handle->flag.isFirstRiseEdge == ON && handle->flag.isSecondRiseEdge == OFF)
        {
            
            handle->CCR.secondEdgeRise_CCR = __HAL_TIM_GET_COMPARE(handle->conf.htim, handle->conf.RiseChannel);
			__HAL_TIM_CLEAR_FLAG(handle->conf.htim, TIM_FLAG_CC1);
            handle->flag.isSecondRiseEdge = ON;
        }
    }

    if (htim->Channel == handle->callBack_param.FallChannel)
    {
        __HAL_TIM_CLEAR_FLAG(handle->conf.htim, TIM_FLAG_CC2);
        handle->CCR.FallEdge_CCR = __HAL_TIM_GET_COMPARE(handle->conf.htim, handle->conf.FallChannel);
        handle->flag.isFallEdge = ON;
    }

    if (handle->flag.isFallEdge == ON && handle->flag.isSecondRiseEdge == ON && handle->flag.isFirstRiseEdge == ON)
    {
        memset((void *)(&handle->flag), OFF, sizeof(pwm_Capture_Flag_t)); // 清空标志位开始计算

        /** 开始计算 */

        // 周期
        handle->result.cycle = (handle->CCR.secondEdgeRise_CCR + handle->CCR.FallEdge_CCR);

        // 占空比
       // if (handle->CCR.secondEdgeRise_CCR != handle->CCR.firstEdgeRise_CCR)
        {
            handle->result.duty = ((float)handle->CCR.FallEdge_CCR / 999) * 100;
        }

        // 频率
        handle->result.freq = 1 / ((handle->result.cycle) * 1E-6);
		handle->flag.isCapComplete = ON;
		memset((void *)&handle->CCR,0,sizeof(pwm_Capture_Int_t));
    }
}

/**
 * @brief 停止PWM捕获输入
 * 
 * @param handle 
 */
void pwmCapture_Stop(pwm_Capture_Handle_t *handle)
{
    __HAL_TIM_CLEAR_FLAG(handle->conf.htim, TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG(handle->conf.htim, TIM_FLAG_CC2);
	handle->flag.isCapComplete = OFF;
    HAL_TIM_IC_Stop_IT(handle->conf.htim, handle->conf.RiseChannel);
    HAL_TIM_IC_Stop_IT(handle->conf.htim, handle->conf.FallChannel);
}

/**
 * @brief 重启复位pwm捕获输入
 * 
 * @param handle 
 */
void pwmCapture_Reset(pwm_Capture_Handle_t *handle)
{

    __HAL_TIM_CLEAR_FLAG(handle->conf.htim, TIM_FLAG_CC1);
    HAL_TIM_IC_Start_IT(handle->conf.htim, handle->conf.RiseChannel);
    HAL_TIM_IC_Start_IT(handle->conf.htim, handle->conf.FallChannel);
}
