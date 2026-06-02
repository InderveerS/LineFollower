/*
 * sensors.c
 *
 *  Created on: June 2, 2026
 *      Author: inder
 */

#include "sensors.h"
#include "timing.h"
#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

// timer 3 is for left side, timer 4 is on right

// Motor encoder constants
#define CPR 408.0f // accounts for gearing
// #define COUNTER_MAX 65535 // not needed for now
#define WHEEL_DIAMETER 0.025f// in m
#define PI 3.14159265359f

static const float circumference = PI * WHEEL_DIAMETER;
static uint16_t leftTickPrevious = 0;
static uint16_t rightTickPrevious = 0;

// IR sensor values
#define NUM_PHOTOTRANSISTORS 5
__attribute__((aligned(4)))
static volatile uint16_t adcBuff[NUM_PHOTOTRANSISTORS]; // buffer to store adc values

static uint16_t thresholdValues[NUM_PHOTOTRANSISTORS] = {
	1000, 1000, 1000, 1000, 1000
}; // threshold values from left --> right
static float weighting[NUM_PHOTOTRANSISTORS] = {
	-2.0f, -1.0f, 0.0f, 1.0f, 2.0f
};

// assume left side is negative, check later
float getLeftVel(void) {
	uint16_t currentTick = TIM3->CNT;
	int16_t delta = (int16_t) (currentTick - leftTickPrevious);

	leftTickPrevious = currentTick;
	float ticksPerS = (float) delta/getDTM();

	return -(ticksPerS / CPR) * circumference;
}

float getRightVel(void) {
	uint16_t currentTick = TIM4->CNT;
	int16_t delta = (int16_t) (currentTick - rightTickPrevious);

	rightTickPrevious = currentTick;
	float ticksPerS = (float) delta/getDTM();

	return (ticksPerS / CPR) * circumference;
}

void irBuffInit(void) {
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adcBuff, NUM_PHOTOTRANSISTORS);
}

float getLineError(void) {
	float error = 0.0f;

	for(int i = 0; i < NUM_PHOTOTRANSISTORS; i++) {
		if(adcBuff[i] < thresholdValues[i]) {
			error += weighting[i];
		}
	}

	return error;
}


