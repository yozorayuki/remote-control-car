#ifndef _FINAL_CHARGE_H_
#define _FINAL_CHARGE_H_

#include "precompile.h"

#define INNER_TIME 900
#define FORWARD_I_TIME 675
#define OUTER_TIME_I 750
#define OUTER_TIME_II 250
#define FORWARD_II_TIME 750

#define INNER_DIS 15000
#define FORWARD_I_DIS 2500
#define FORWARD_II_DIS 5000

#define INTERVAL 85

void pinDef(uint8_t dir);
void correctDir(uint8_t dir);
void turnInner(uint8_t dir);
void fowardToSlope(void);
void turnOuter(uint8_t dir);
void upwardSlope(void);
void Final_Charge (uint8_t _direction);

#endif
