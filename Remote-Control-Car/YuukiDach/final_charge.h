#ifndef _FINAL_CHARGE_H_
#define _FINAL_CHARGE_H_

#include "precompile.h"

#define TURN_PWM 18       // When the power of the battery if full even 20 may seem to be too fast.

#define INNER_TIME 100
#define FORWARD_I_TIME 700
#define OUTER_TIME_I 800
#define FORWARD_II_TIME 870

#define CORRECT_DIS 30000
#define INNER_DIS 15000
#define FORWARD_I_DIS 2500
#define FORWARD_II_DIS 4100

#define INTERVAL 85

void pinDef(uint8_t dir);
void correctDir(uint8_t dir);
void turnInner(uint8_t dir);
void fowardToSlope(uint8_t dir);
void turnOuter(uint8_t dir);
void upwardSlope(uint8_t dir);
void Final_Charge (uint8_t _direction);

#endif
