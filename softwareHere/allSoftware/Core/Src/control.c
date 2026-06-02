/*
 * control.c
 *
 *  Created on: May 21, 2026
 *      Author: inder
 */

#include "control.h"
#include "pid.h"
#include "motors.h"
#include "sensors.h"
#include "timing.h"
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

// Steer PID coefficients
#define s_Kp 0.0f
#define s_Ki 0.0f
#define s_Kd 0.0f

// Steer PID maximum/minimum (in angular velocity of robot (w))
#define STEER_MAX_LEFT 16.8f
#define STEER_MAX_RIGHT -16.8f
#define S_ALPHA 0.5f

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
	omega = updatePID(&steerPID, 0.0f, getLineError(), getDTS());
}

void updateMotors(void) {
	// Inverse kinematics --> find exact needed speed for turn
	uint16_t leftTargetVel = baseVel - omega * (WHEELBASE/2.0f);
	uint16_t rightTargetVel = baseVel + omega * (WHEELBASE/2.0f);

	float leftCurrentVel = getLeftVel();
	float rightCurrentVel = getRightVel();

	float leftDutySet = updatePID(&leftMotorPID, (float) leftTargetVel, leftCurrentVel, getDTM());
	float rightDutySet = updatePID(&rightMotorPID, (float) rightTargetVel, rightCurrentVel, getDTM());

	spinLeftMotor((int32_t) roundf(leftDutySet));
	spinRightMotor((int32_t) roundf(rightDutySet));
}
