/*
 * control.c
 *
 *  Created on: May 21, 2026
 *      Author: inder
 */

#include "control.h"
#include "pid.h"
#include "motors.h"
#include <math.h>

// Right motor PID coefficients
#define RM_Kp 0.0f
#define RM_Ki 0.0f
#define RM_Kd 0.0f

// Left motor PID coefficients
#define LM_Kp 0.0f
#define LM_Ki 0.0f
#define LM_Kd 0.0f

// maximum PWM outputs
#define MOTOR_MAX 100
#define MOTOR_MIN -100
#define M_ALPHA 0.5f
#define dtM 0.0001f

// Steer PID coefficients
#define s_Kp 0.0f
#define s_Ki 0.0f
#define s_Kd 0.0f

// Steer PID maximum/minimum (in angular velocity of robot (w))
#define STEER_MAX_LEFT 16.8f
#define STEER_MAX_RIGHT -16.8f
#define S_ALPHA 0.5f
#define dtS 0.001f

// Characteristics of the robot
#define WHEELBASE 0.048f

// Global outer loop variables
volatile int16_t omega = 0;
volatile uint16_t baseVel = 400;

// PID Controllers
static PIDController rightMotorPID;
static PIDController leftMotorPID;
static PIDController steerPID;

void controllerInit(void) {
	initPID(&rightMotorPID, RM_Kp, RM_Ki, RM_Kd, MOTOR_MIN, MOTOR_MAX, M_ALPHA);
	initPID(&leftMotorPID, LM_Kp, LM_Ki, LM_Kd, MOTOR_MIN, MOTOR_MAX, M_ALPHA);
	initPID(&steerPID, s_Kp, s_Ki, s_Kd, STEER_MAX_RIGHT, STEER_MAX_LEFT, S_ALPHA);
}

// outer loop --> uses IR sensors to output
void updateSteerControl(void) {
	// TODO: add sensors
	float error = 0.0f;

	omega = updatePID(&steerPID, 0.0f, error, dtS);
}

void updateMotors(void) {
	// Inverse kinematics --> find exact needed speed for turn
	uint16_t leftTargetVel = baseVel - omega * (WHEELBASE/2.0f);

	// TODO: add currentVel once sensors file is done
	float dutySet = updatePID(&leftMotorPID, (float) leftTargetVel, currentVel, dtM);
	spinLeftMotor((int32_t) roundf(dutySet));
}
