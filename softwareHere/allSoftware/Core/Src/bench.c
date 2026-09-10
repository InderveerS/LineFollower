/*
 * bench.c
 *
 *  Created on: Sep 6, 2026
 *      Author: inder
 */

#include "bench.h"
#include "control.h"
#include "motors.h"
#include "sensors.h"
#include "sender.h"
#include "stm32f4xx_hal.h"

// Status LEDs and buttons
#define LED1_PORT GPIOC
#define LED1_PIN GPIO_PIN_12
#define LED2_PORT GPIOD
#define LED2_PIN GPIO_PIN_2
#define SW1_PORT GPIOB
#define SW1_PIN GPIO_PIN_9
#define SW2_PORT GPIOB
#define SW2_PIN GPIO_PIN_8

#define DEBOUNCE_TICKS 10

// Countdown before a test starts, ticks at 200 Hz
#define ARM_TICKS 400
#define BLINK_FAST 20
#define BLINK_SLOW 100

// Shared row buffer
#define LOG_ROWS 400
#define LOG_FIELDS 5

// Duty sweep settings
#define SWEEP_STEP 5
#define SWEEP_SETTLE_TICKS 40
#define SWEEP_AVG_TICKS 20

// Calibration duration, ticks at 200 Hz
#define CAL_TICKS 2000

// For tuning test, ticks at 200 Hz
#define STEP_BASE_TICKS 60
#define STEP_HOLD_TICKS 240
#define STEP_TAIL_TICKS 100

volatile RobotMode robotMode = MODE_IDLE;
volatile uint8_t activeTest = ACTIVE_TEST;
volatile int32_t testDuty = 0;
volatile float stepLow = 0.2f;
volatile float stepHigh = 0.4f;

static int16_t logBuf[LOG_ROWS][LOG_FIELDS];
static uint32_t rowCount = 0;
static uint8_t logFields = 3;
static const char *logHeader = "";

static int32_t sweepDuty = 0;
static int32_t sweepDir = SWEEP_STEP;
static uint32_t sweepTick = 0;
static float leftAccum = 0.0f;
static float rightAccum = 0.0f;
static uint32_t accumCount = 0;

static uint32_t stepTick = 0;
static uint32_t calTick = 0;
static uint32_t armTick = 0;
static uint32_t dumpRow = 0;
static uint32_t blinkTick = 0;

// Set when a test finishes with rows worth dumping
static uint8_t dataPending = 0;

static void setLed1(uint8_t on) {
	HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void setLed2(uint8_t on) {
	HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Off: idle, slow blink: data is waiting, fast blink: busy, solid: testing
static uint8_t led1State(void) {
	switch (robotMode) {
	case MODE_SWEEP:
	case MODE_STEP:
	case MODE_CALIBRATE:
		return 1;
	case MODE_ARMED:
	case MODE_DUMP:
		return (blinkTick / BLINK_FAST) % 2;
	case MODE_IDLE:
		return dataPending ? ((blinkTick / BLINK_SLOW) % 2) : 0;
	default:
		return 0;
	}
}

// Debounced button
static uint8_t pressed(GPIO_TypeDef *port, uint16_t pin, uint8_t *count) {
	if (HAL_GPIO_ReadPin(port, pin) != GPIO_PIN_RESET) {
		*count = 0;
		return 0;
	}

	if (*count >= DEBOUNCE_TICKS)
		return 0;

	(*count)++;
	return (*count == DEBOUNCE_TICKS);
}

static void logRow(int16_t a, int16_t b, int16_t c, int16_t d, int16_t e) {
	if (rowCount >= LOG_ROWS)
		return;

	logBuf[rowCount][0] = a;
	logBuf[rowCount][1] = b;
	logBuf[rowCount][2] = c;
	logBuf[rowCount][3] = d;
	logBuf[rowCount][4] = e;
	rowCount++;
}

static void startTest(void) {
	rowCount = 0;

	if (activeTest == TEST_CALIBRATE) {
		logFields = 3;
		logHeader = "sensor,min,max";
		calibrateReset();
		setMode(MODE_CALIBRATE);
		return;
	}

	if (activeTest == TEST_STEP) {
		logFields = 5;
		logHeader = "target_mmps,left_mmps,right_mmps,left_duty,right_duty";
		setMode(MODE_STEP);
		return;
	}

	logFields = 3;
	logHeader = "duty,left_mmps,right_mmps";
	sweepDuty = 0;
	sweepDir = SWEEP_STEP;
	setMode(MODE_SWEEP);
}

static void armUpdate(void) {
	spinCoast();
	armTick++;

	if (armTick >= ARM_TICKS)
		startTest();
}

// Steps duty up to 100 then back down, averaging speed at each step
static void sweepUpdate(void) {
	spinPercent(sweepDuty);
	sampleVelocities();
	sweepTick++;

	if (sweepTick > SWEEP_SETTLE_TICKS) {
		leftAccum += lastLeftVel;
		rightAccum += lastRightVel;
		accumCount++;
	}

	if (sweepTick < SWEEP_SETTLE_TICKS + SWEEP_AVG_TICKS)
		return;

	if (accumCount > 0)
		logRow((int16_t) sweepDuty,
				(int16_t) (leftAccum / accumCount * 1000.0f),
				(int16_t) (rightAccum / accumCount * 1000.0f), 0, 0);

	sweepTick = 0;
	leftAccum = 0.0f;
	rightAccum = 0.0f;
	accumCount = 0;
	sweepDuty += sweepDir;

	if (sweepDuty > 100) {
		sweepDuty = 100 - SWEEP_STEP;
		sweepDir = -SWEEP_STEP;
	} else if (sweepDuty < 0) {
		dataPending = 1;
		setMode(MODE_IDLE);
	}
}

// Sweep the array over the line for the whole window, then store the bounds
static void calUpdate(void) {
	spinCoast();
	calibrate();
	calTick++;

	if (calTick < CAL_TICKS)
		return;

	for (int i = 0; i < NUM_PHOTOTRANSISTORS; i++)
		logRow(i, getCalMin(i), getCalMax(i), 0, 0);

	dataPending = 1;
	setMode(MODE_IDLE);
}

// Holds stepLow, steps to stepHigh, drops back, logging every tick
static void stepUpdate(void) {
	float target = stepLow;

	if (stepTick >= STEP_BASE_TICKS && stepTick < STEP_BASE_TICKS + STEP_HOLD_TICKS)
		target = stepHigh;

	updateMotors(target, target);

	logRow((int16_t) (target * 1000.0f),
			(int16_t) (lastLeftVel * 1000.0f),
			(int16_t) (lastRightVel * 1000.0f),
			(int16_t) lastLeftDuty,
			(int16_t) lastRightDuty);

	stepTick++;

	if (stepTick >= STEP_BASE_TICKS + STEP_HOLD_TICKS + STEP_TAIL_TICKS) {
		dataPending = 1;
		setMode(MODE_IDLE);
	}
}

// One line per tick so the blocking send never overruns the tick
static void dumpUpdate(void) {
	spinCoast();

	if (dumpRow == 0) {
		sendf("%s\r\n", logHeader);
		dumpRow = 1;
		return;
	}

	if (dumpRow <= rowCount) {
		int16_t *r = logBuf[dumpRow - 1];

		if (logFields == 3)
			sendf("%d,%d,%d\r\n", r[0], r[1], r[2]);
		else
			sendf("%d,%d,%d,%d,%d\r\n", r[0], r[1], r[2], r[3], r[4]);

		dumpRow++;
		return;
	}

	sendf("EOF\r\n");
	dataPending = 0;
	setMode(MODE_IDLE);
}

void setMode(RobotMode m) {
	spinCoast();
	controlReset();
	resetVelocity();

	sweepTick = 0;
	stepTick = 0;
	calTick = 0;
	armTick = 0;
	dumpRow = 0;
	leftAccum = 0.0f;
	rightAccum = 0.0f;
	accumCount = 0;

	robotMode = m;
}

// SW1 walks the bench flow, SW2 toggles the real loop
static void handleSw1(void) {
	if (robotMode == MODE_RUN) {
		calibrateReset();
		dataPending = 0;
		setMode(MODE_IDLE);
		return;
	}

	if (robotMode != MODE_IDLE) {
		if (rowCount > 0)
			dataPending = 1;
		setMode(MODE_IDLE);
		return;
	}

	setMode(dataPending ? MODE_DUMP : MODE_ARMED);
}

static void handleSw2(void) {
	setMode(robotMode == MODE_RUN ? MODE_IDLE : MODE_RUN);
}

void benchTick(void) {
	static uint8_t sw1Count = 0;
	static uint8_t sw2Count = 0;

	blinkTick++;

	if (pressed(SW1_PORT, SW1_PIN, &sw1Count))
		handleSw1();

	if (pressed(SW2_PORT, SW2_PIN, &sw2Count))
		handleSw2();

	switch (robotMode) {
	case MODE_ARMED:
		armUpdate();
		break;
	case MODE_SWEEP:
		sweepUpdate();
		break;
	case MODE_STEP:
		stepUpdate();
		break;
	case MODE_CALIBRATE:
		calUpdate();
		break;
	case MODE_DUMP:
		dumpUpdate();
		break;
	case MODE_OPEN_LOOP:
		spinPercent(testDuty);
		sampleVelocities();
		break;
	case MODE_RUN:
		break;
	default:
		spinCoast();
		break;
	}

	setLed1(led1State());
	setLed2(robotMode == MODE_RUN);
}
