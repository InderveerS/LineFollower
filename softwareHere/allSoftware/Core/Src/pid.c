/*
 * pid.c
 *
 *  Created on: May 8, 2026
 *      Author: inder
 */


#include "pid.h"

void initPID(PIDController *pid, float kp, float ki, float kd,
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

void resetPID(PIDController *pid) {
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

float updatePID(PIDController *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;

    // Proportional
    float p = pid->kp * error;

    // Derivative on measurement (avoids derivative kick on setpoint change)
    // IIR low-pass filter applied to the derivative term
    float rawDerivative = -(measurement - pid->prev_meas) / dt;
    float d = pid->alpha * rawDerivative + (1.0f - pid->alpha) * pid->prev_deriv;
    pid->prev_deriv = d;
    pid->prev_meas  = measurement;
    d = pid->kd * d;

    // Anti windup, only block integration that pushes further into saturation
    float trial = pid->integral + error * dt;
    float trialOutput = p + pid->ki * trial + d;

    int saturated = (trialOutput > pid->out_max) || (trialOutput < pid->out_min);
    int drivingDeeper = (trialOutput * error) > 0.0f;

    if (!(saturated && drivingDeeper))
        pid->integral = trial;

    float output = p + pid->ki * pid->integral + d;

    return clamp(output, pid->out_min, pid->out_max);
}
