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


// Motor encoder constants
#define CPR 408.0f // accounts for gearing
// #define COUNTER_MAX 65535 // not needed for now
#define WHEEL_DIAMETER 0.025f// in m
#define PI 3.14159265359f

#define VEL_WINDOW 4

static const float circumference = PI * WHEEL_DIAMETER;
static uint16_t leftTickHist[VEL_WINDOW];
static uint16_t rightTickHist[VEL_WINDOW];
static uint8_t leftHistIdx = 0;
static uint8_t rightHistIdx = 0;
static uint8_t leftSeeded = 0;
static uint8_t rightSeeded = 0;

// IR sensor values
__attribute__((aligned(4)))
static volatile uint16_t adcBuff[NUM_PHOTOTRANSISTORS]; // buffer to store adc values

#define LINE_PRESENT_THRESHOLD 750
#define MAX_ERROR 3.5f
#define ADC_MAX 4095
int8_t lastDirection = 0;

//static uint16_t thresholdValues[NUM_PHOTOTRANSISTORS] = {
//	1000, 1000, 1000, 1000, 1000
//}; // threshold values from left --> right
static float weighting[NUM_PHOTOTRANSISTORS] = {
	-2.0f, -1.0f, 0.0f, 1.0f, 2.0f
};

// Filled in by calibrate(), min is the white surface and max is the line
static uint16_t minVals[NUM_PHOTOTRANSISTORS] = {
	ADC_MAX, ADC_MAX, ADC_MAX, ADC_MAX, ADC_MAX
};

static uint16_t maxVals[NUM_PHOTOTRANSISTORS] = {
	0, 0, 0, 0, 0
};

static uint16_t normalize(uint16_t value, uint16_t min, uint16_t max) {
    if (max <= min) {
        return 0;
    }
    else if (value < min) {
        return 0;
    } else if (value > max) {
        return 1000;
    } else {
        return (value - min) * 1000 / (max - min);
    }
}

// timer 3 is for left side, timer 4 is on right

static float windowedVel(uint16_t now, uint16_t *hist, uint8_t *idx, uint8_t *seeded) {
	if (!*seeded) {
		for (int i = 0; i < VEL_WINDOW; i++)
			hist[i] = now;
		*seeded = 1;
	}

	uint16_t then = hist[*idx];
	hist[*idx] = now;
	*idx = (*idx + 1) % VEL_WINDOW;

	int16_t delta = (int16_t) (now - then);
	float ticksPerS = (float) delta / (VEL_WINDOW * getDTM());

	return (ticksPerS / CPR) * circumference;
}

void resetVelocity(void) {
	leftSeeded = 0;
	rightSeeded = 0;
}

float getLeftVel(void) {
	return -windowedVel(TIM3->CNT, leftTickHist, &leftHistIdx, &leftSeeded);
}

float getRightVel(void) {
	return windowedVel(TIM4->CNT, rightTickHist, &rightHistIdx, &rightSeeded);
}

void irBuffInit(void) {
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adcBuff, NUM_PHOTOTRANSISTORS);
}

float getLineError(void) {
	float error = 0.0f;
	float totalReading = 0.0f;

	for (int i = 0; i < NUM_PHOTOTRANSISTORS; i++) {
		float reading = normalize(adcBuff[i], minVals[i], maxVals[i]);
		totalReading += reading;
		error += reading * weighting[i];
	}

	if (totalReading > LINE_PRESENT_THRESHOLD) {
		lastDirection = (error < 0.0f) ? -1 : 1;
		return error / totalReading;
	}

	return lastDirection * MAX_ERROR;
}

float getRawIR(uint8_t index) {
	return adcBuff[index];
}

void calibrateReset(void) {
	for (int i = 0; i < NUM_PHOTOTRANSISTORS; i++) {
		minVals[i] = ADC_MAX;
		maxVals[i] = 0;
	}
}

void calibrate(void) {
	for (int i = 0; i < NUM_PHOTOTRANSISTORS; i++) {
		uint16_t value = adcBuff[i];

		if (value < minVals[i])
			minVals[i] = value;
		else if (value > maxVals[i])
			maxVals[i] = value;
	}
}

uint16_t getCalMin(uint8_t index) {
	return minVals[index];
}

uint16_t getCalMax(uint8_t index) {
	return maxVals[index];
}
