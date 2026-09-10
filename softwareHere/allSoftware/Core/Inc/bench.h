/*
 * bench.h
 *
 *  Created on: Sep 6, 2026
 *      Author: inder
 */

#ifndef INC_BENCH_H_
#define INC_BENCH_H_

#include <stdint.h>

// Which test SW1 runs
#define TEST_SWEEP 0
#define TEST_STEP 1
#define TEST_CALIBRATE 2
#define ACTIVE_TEST TEST_CALIBRATE

typedef enum {
	MODE_IDLE = 0,
	MODE_RUN,
	MODE_OPEN_LOOP,
	MODE_ARMED,
	MODE_SWEEP,
	MODE_STEP,
	MODE_CALIBRATE,
	MODE_DUMP
} RobotMode;

extern volatile RobotMode robotMode;
extern volatile uint8_t activeTest;

// Open loop duty
extern volatile int32_t testDuty;

// Which velocities to run PI tuning test at
extern volatile float stepLow;
extern volatile float stepHigh;

// Enters new mode
void setMode(RobotMode m);

// polls buttons and runs active test tick
void benchTick(void);

#endif /* INC_BENCH_H_ */
