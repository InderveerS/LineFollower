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
#include <stdint.h>

// Motor PID coefficients, shared by both wheels
volatile float M_Kp = 58.0f;
volatile float M_Ki = 300.0f;
volatile float M_Kd = 0.0f;

// maximum PWM outputs (less than 100 because of ff)
#define MOTOR_MAX 30
#define MOTOR_MIN -30
#define M_ALPHA 0.0f

#define FF_SLOPE 116.78f
#define FF_OFFSET 4.35f

// for testing PI controller response (set 0.7), 1.0 for normal use
volatile float ffScale = 1.0f;

volatile float lastLeftVel = 0.0f;
volatile float lastRightVel = 0.0f;
volatile float lastLeftDuty = 0.0f;
volatile float lastRightDuty = 0.0f;

// Steer PID coefficients
volatile float s_Kp = 5.5f;
volatile float s_Ki = 0.0f;
volatile float s_Kd = 0.0f;

// Characteristics of the robot
const float WHEELBASE = 0.048f;

// Min Percent for robot to move (linearize curve)
// #define PERCENT_MIN 10

// Global outer loop variables
volatile float omega = 0.0f;
const float baseVel = 0.5f;
const float vMax = 0.8f;

// Steer PID maximum/minimum (in angular velocity of robot (w))
const float STEER_MAX_LEFT = (vMax - baseVel) / (WHEELBASE/2);
const float STEER_MAX_RIGHT = -(vMax - baseVel) / (WHEELBASE/2);
#define S_ALPHA 0.2f


// PID Controllers
static PIDController rightMotorPID;
static PIDController leftMotorPID;
static PIDController steerPID;

void controlReset(void) {
	initPID(&rightMotorPID, M_Kp, M_Ki, M_Kd, MOTOR_MIN, MOTOR_MAX, M_ALPHA);
	initPID(&leftMotorPID, M_Kp, M_Ki, M_Kd, MOTOR_MIN, MOTOR_MAX, M_ALPHA);
	initPID(&steerPID, s_Kp, s_Ki, s_Kd, STEER_MAX_RIGHT, STEER_MAX_LEFT, S_ALPHA);
}

void controllerInit(void) {
	controlReset();
}

// outer loop --> uses IR sensors to output
void updateSteerControl(void) {
	omega = updatePID(&steerPID, 0.0f, getLineError(), getDTS());
}

static float feedforward(float v) {
	if (v > 0.0f)
		return ffScale * (FF_SLOPE * v + FF_OFFSET);
	if (v < 0.0f)
		return ffScale * (FF_SLOPE * v - FF_OFFSET);
	return 0.0f;
}

void sampleVelocities(void) {
	lastLeftVel = getLeftVel();
	lastRightVel = getRightVel();
}

void updateMotors(float leftTarget, float rightTarget) {
	sampleVelocities();

	lastLeftDuty = feedforward(leftTarget)
			+ updatePID(&leftMotorPID, leftTarget, lastLeftVel, getDTM());
	lastRightDuty = feedforward(rightTarget)
			+ updatePID(&rightMotorPID, rightTarget, lastRightVel, getDTM());

	spinLeftMotor(lastLeftDuty);
	spinRightMotor(lastRightDuty);
}

void controlTick(void) {
	static uint32_t tick = 0;
	tick++;

	if (tick % 4 == 0)
		updateSteerControl();

	updateMotors(baseVel - omega * (WHEELBASE/2.0f),
			baseVel + omega * (WHEELBASE/2.0f));
}
