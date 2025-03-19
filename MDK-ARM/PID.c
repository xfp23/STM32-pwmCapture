#include "PID.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

void PIDController_Init(PIDController_Handle_t *handle, PIDController_Conf_t *conf)
{
	if (handle == NULL || *handle != NULL)  // 🚨 需要判断 *handle 是否已经被分配过
	{
		return;
	}

	*handle = calloc(1, sizeof(PIDController_Class_t)); // ✅ 这样分配的内存才会被正确存入调用者的 handle
	if (*handle == NULL)
	{
		return;
	}

	// 赋值
	(*handle)->Kp = conf->kp;
	(*handle)->Ki = conf->ki;
	(*handle)->Kd = conf->kd;
	(*handle)->tau = conf->tau;
	(*handle)->limMin = conf->limMin;
	(*handle)->limMax = conf->limMax;
	(*handle)->limMinInt = conf->limMinInt;
	(*handle)->limMaxInt = conf->limMaxInt;
	(*handle)->T = conf->T;

	// 初始化状态变量
	(*handle)->integrator = 0.0f;
	(*handle)->prevError = 0.0f;
	(*handle)->differentiator = 0.0f;
	(*handle)->prevMeasurement = 0.0f;
	(*handle)->out = 0.0f;
}


float PIDController_Update(PIDController_Handle_t *handle, float setpoint, float measurement)
{
	if(handle == NULL || *handle == NULL) return 0;
	/*
	 * Error signal
	 */
	float error = setpoint - measurement;

	/*
	 * Proportional
	 */
	float proportional = (*handle)->Kp * error;

	/*
	 * Integral
	 */
	(*handle)->integrator = (*handle)->integrator + 0.5f * (*handle)->Ki * (*handle)->T * (error + (*handle)->prevError);

	/* Anti-wind-up via integrator clamping */
	if ((*handle)->integrator > (*handle)->limMaxInt)
	{

		(*handle)->integrator = (*handle)->limMaxInt;
	}
	else if ((*handle)->integrator < (*handle)->limMinInt)
	{

		(*handle)->integrator = (*handle)->limMinInt;
	}

	/*
	 * Derivative (band-limited differentiator)
	 */

	(*handle)->differentiator = -(2.0f * (*handle)->Kd * (measurement - (*handle)->prevMeasurement) /* Note: derivative on measurement, therefore minus sign in front of equation! */
								  + (2.0f * (*handle)->tau - (*handle)->T) * (*handle)->differentiator) /
								(2.0f * (*handle)->tau + (*handle)->T);

	/*
	 * Compute output and apply limits
	 */
	(*handle)->out = proportional + (*handle)->integrator + (*handle)->differentiator;

	if ((*handle)->out > (*handle)->limMax)
	{

		(*handle)->out = (*handle)->limMax;
	}
	else if ((*handle)->out < (*handle)->limMin)
	{

		(*handle)->out = (*handle)->limMin;
	}

	/* Store error and measurement for later use */
	(*handle)->prevError = error;
	(*handle)->prevMeasurement = measurement;

	/* Return controller output */
	return (*handle)->out;
}
