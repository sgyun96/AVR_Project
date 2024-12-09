#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "def.h"

#define PIR_PIN     PORTC0
#define LED_PIN     PORTC1

void sensorInit(void);
uint8_t isMotion(void);
void motLedOn(void);
void motLedOff(void);
void motHandle(void);
 
#endif