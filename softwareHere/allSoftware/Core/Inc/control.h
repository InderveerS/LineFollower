/*
 * control.h
 *
 *  Created on: May 21, 2026
 *      Author: inder
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include <stdint.h>

// Tunable at runtime
extern volatile float M_Kp;
extern volatile float M_Ki;
extern volatile float M_Kd;
extern volatile float s_Kp;
extern volatile float s_Ki;
extern volatile float s_Kd;

// 0 disables feedforward, 1 is for real run
extern volatile float ffScale;

// Last values seen by the motor loop, in m/s and percent
extern volatile float lastLeftVel;
extern volatile float lastRightVel;
extern volatile float lastLeftDuty;
extern volatile float lastRightDuty;

void controllerInit(void);
void updateSteerControl(void);
void sampleVelocities(void);

/*
 * @param: leftTarget, rightTarget - wheel speeds to hold in m/s
 * @returns: void
 * @brief: runs one pass of the velocity loop on both wheels
*/
void updateMotors(float leftTarget, float rightTarget);

/*
 * @returns: void
 * @brief: runs one pass of the cascade, steer loop every 4th call
*/
void controlTick(void);

/*
 * @returns: void
 * @brief: reloads the gains and clears all integrator and derivative state
*/
void controlReset(void);

#endif /* INC_CONTROL_H_ */
