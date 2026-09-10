/*
 * sensors.h
 *
 *  Created on: June 2, 2026
 *      Author: inder
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include <stdint.h>

#define NUM_PHOTOTRANSISTORS 5

void resetVelocity(void);
float getLeftVel(void);
float getRightVel(void);
void irBuffInit(void);
float getLineError(void);
float getRawIR(uint8_t index);
void calibrate(void); // run across line to calibrate IR phototransistors
void calibrateReset(void);

uint16_t getCalMin(uint8_t index);
uint16_t getCalMax(uint8_t index);

#endif /* INC_SENSORS_H_ */
