/*
 * pid.h
 *
 *  Created on: May 8, 2026
 *      Author: inder
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct {
    // Gains
    float kp;
    float ki;
    float kd;

    // Output limits
    float out_min;
    float out_max;

    // IIR LPF coefficient for derivative (0 < alpha <= 1)
    float alpha;

    // State
    float integral;
    float prev_meas;
    float prev_deriv;
} PIDController;

void  PID_Init  (PIDController *pid, float kp, float ki, float kd,
                 float out_min, float out_max, float lpf_alpha);
void  PID_Reset (PIDController *pid);
float PID_Update(PIDController *pid, float setpoint, float measurement, float dt);

#endif /* INC_PID_H_ */
