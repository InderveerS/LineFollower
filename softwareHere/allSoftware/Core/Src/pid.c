/*
 * pid.c
 *
 *  Created on: May 8, 2026
 *      Author: inder
 */


#include "pid.h"

void PID_Init(PIDController *pid, float kp, float ki, float kd,
              float out_min, float out_max, float lpf_alpha) {
    pid->kp        = kp;
    pid->ki        = ki;
    pid->kd        = kd;
    pid->out_min   = out_min;
    pid->out_max   = out_max;
    pid->alpha     = lpf_alpha;  // 0 < alpha <= 1; lower = more filtering
    pid->integral  = 0.0f;
    pid->prev_meas = 0.0f;
    pid->prev_deriv = 0.0f;
}

void PID_Reset(PIDController *pid) {
    pid->integral   = 0.0f;
    pid->prev_meas  = 0.0f;
    pid->prev_deriv = 0.0f;
}

static float clamp(float val, float min, float max) {
    if (val > max)
    	return max;
    if (val < min)
    	return min;
    return val;
}

float PID_Update(PIDController *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;

    // Proportional
    float p = pid->kp * error;

    // Integral
    float i = pid->ki * pid->integral;

    // Derivative on measurement (avoids derivative kick on setpoint change)
    // IIR low-pass filter applied to the derivative term
    float rawDerivative = -(measurement - pid->prev_meas) / dt;
    float d = pid->alpha * rawDerivative + (1.0f - pid->alpha) * pid->prev_deriv;
    pid->prev_deriv = d;
    pid->prev_meas  = measurement;
    d = pid->kd * d;

    float output = p + i + d;

    // Anti windup for integral
    if(output < pid->out_max && output > pid->out_min)
        pid->integral += error * dt;

    return clamp(output, pid->out_min, pid->out_max);
}
