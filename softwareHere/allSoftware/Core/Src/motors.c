/*
 * motors.c
 *
 *  Created on: May 8, 2026
 *      Author: inder
 */

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim1;

#define MAX_PWM 8400

void spinCoast(void) {
	htim1.Instance->CCR1 = 0;
	htim1.Instance->CCR2 = 0;
	htim1.Instance->CCR3 = 0;
	htim1.Instance->CCR4 = 0;
}

void spinBrake(void) {
	htim1.Instance->CCR1 = MAX_PWM;
	htim1.Instance->CCR2 = MAX_PWM;
	htim1.Instance->CCR3 = MAX_PWM;
	htim1.Instance->CCR4 = MAX_PWM;
}

/* @param: dutyPercent - percentage of PWM duty cycle to run at (-100 -> 100). Sets it to 0
 * if out of range
 * @returns: void
 * @brief: runs both motors at percent duty cycle given, positive for forward
 * and negative for backwards
*/
void spinPercent(int dutyPercent) {
	if (dutyPercent > 100 || dutyPercent < -100)
		dutyPercent = 0;

	if (dutyPercent > 0) {
		uint32_t ccr = (MAX_PWM * dutyPercent) / 100;
		htim1.Instance->CCR1 = ccr;
		htim1.Instance->CCR2 = 0;
		htim1.Instance->CCR3 = ccr;
		htim1.Instance->CCR4 = 0;
	}
	else if (dutyPercent < 0) {
		uint32_t ccr = (MAX_PWM * -dutyPercent) / 100;
		htim1.Instance->CCR1 = 0;
		htim1.Instance->CCR2 = ccr;
		htim1.Instance->CCR3 = 0;
		htim1.Instance->CCR4 = ccr;
	}
	else {
		spinCoast();
	}
}
/* c1 -> left1
 * c2 -> left2
 * c3 -> right2
 * c4 -> right1
*/
